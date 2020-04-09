/**
 * Copyright 2017, 2019 Christian Geldermann
 *
 * This file is part of PeoplezServerLib.
 *
 * PeoplezServerLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PeoplezServerLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PeoplezServerLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von PeoplezServerLib.
 *
 * PeoplezServerLib ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * PeoplezServerLib wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit
 * PeoplezServerLib erhalten haben. Wenn nicht, siehe
 * <http://www.gnu.org/licenses/>.
 */

// Own headers
#include "ConnectionsManager.hpp"

// Local includes
#include "../../Logging/Logger.hpp"

// Extern includes
#include <limits>
extern "C"
{
#include <fcntl.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <unistd.h>
}

/**
 * @def MAXEVENTS
 * @brief Number of events that are fetched per epoll by a thread at once
 */
#define MAXEVENTS 2
/**
 * @def CONNECTION_TIMEOUT
 * @brief Time until inactive connections are removed
 * @details
 * Time interval after that a check for inactive connections is performed.
 * All connections that were inactive since the last check before get removed.
 * So it is possible that a connection stays open for at least 2 * CONNECTIONS_TIMEOUT seconds
 */
#define CONNECTION_TIMEOUT 5
/**
 * @def DIRECT_TIMEOUT
 * @brief Specifies direct or indirect deletion of old entries
 */
#define DIRECT_TIMEOUT

namespace Peoplez
{
	using namespace General::Patterns;

	namespace System
	{
		using namespace Logging;

		namespace IO
		{
			namespace Network
			{
				enum EpollDataType
				{
					CLIENT_INFO,
					LISTENER
				};

				class EpollData
				{
				public:
					EpollData(Listener * const _listener) : type(LISTENER), before(0), next(0), listener(_listener) {}
					EpollData(ClientInfo * const _clientInfo) : type(CLIENT_INFO), before(0), next(0), clientInfo(_clientInfo) {}

					~EpollData()
					{
						if(type == CLIENT_INFO) clientInfo.~__shared_ptr();
						else listener.~__shared_ptr();
					}

					EpollDataType type;

					EpollData * before;
					EpollData * next;

					union
					{
						std::shared_ptr<Listener> listener;
						std::shared_ptr<ClientInfo> clientInfo;
					};
				};

				uint32_t const ConnectionsManager::EPOLL_ERROR_OR_DELETE = EPOLLERR | EPOLLHUP | EPOLLRDHUP;

				ConnectionsManager::ConnectionsManager(Factory & pTFactory, size_t const threads) : clientBuffer(epoll_create1(0)), eventSock(eventfd(0, EFD_NONBLOCK)), listeners(0), newInfos(0), oldInfos(0), perThreadDataFactory(pTFactory), timer(std::bind(&ConnectionsManager::TimeOut, this), std::chrono::seconds(CONNECTION_TIMEOUT))
				{
					try
					{
						//epoll initialization check
						if(clientBuffer == -1) Logger::LogException("Could not create epoll", __FILE__, __LINE__);

						//initialization of event socket for interrupting
						if(eventSock == -1) Logger::LogException("Stop file descriptor not valid", __FILE__, __LINE__);
						//Add(new EventClientInfo(eventSock));
						AddEventSock();

						//Add threads
						AddThreads(threads);

						Logger::LogEvent("ConnectionsManager initialized");
					}
					catch (...)
					{
						Logger::LogException("Error in ConnectionsManger::Initialize", __FILE__, __LINE__);
					}
				}

				void ConnectionsManager::AddEventSock() noexcept
				{
					struct epoll_event event;

					if(MakeSocketNonBlocking(eventSock))
					{
						Logger::LogException("Could not make event socket nonblocking", __FILE__, __LINE__);
						return;
					}

					// Set epoll events to react on
					event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
					// Set socket infos for ThreadPoolFunction
					//event.data.ptr = (void *) eventData;
					event.data.ptr = 0;

					if(epoll_ctl(clientBuffer, EPOLL_CTL_ADD, eventSock, &event) == -1)
					{
						Logger::LogException("Could not add event socekt to clientBuffer", __FILE__, __LINE__);

						// Ensure not to have the socket in epoll buffer
						epoll_ctl(clientBuffer, EPOLL_CTL_DEL, eventSock, 0);
					}
				}

				void ConnectionsManager::Add(ClientInfo * const info) noexcept
				{
					try
					{
						struct epoll_event event;
						int const sock = info->fd;

						// Check validity of socket
						if(sock < 0)
						{
							delete info;
							return;
						}

						// Make socket non-blocking
						if(MakeSocketNonBlocking(sock))
						{
							Logger::LogException("Could not make socket non blocking", __FILE__, __LINE__);
							return;
						}

						// Set epoll events to react on
						event.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
						// Set socket infos for ThreadPoolFunctions
						//event.data.ptr = (void*) info;
						EpollData * const data = new EpollData(info);
						event.data.ptr = (void*) data;

						// Lock ClientInfo lists
						std::unique_lock<std::mutex> const listLock(infoListMutex);

						// Add socket to epoll buffer
						if(epoll_ctl(clientBuffer, EPOLL_CTL_ADD, sock, &event) == -1)
						{
							// Log Error
							Logger::LogException("Could not add new client to clientBuffer", __FILE__, __LINE__);

							// Remove ClientInfo from list of clientInfos
							//DestroyInner(info);

							// Ensure not to have the socket in epoll buffer
							epoll_ctl(clientBuffer, EPOLL_CTL_DEL, sock, 0);

							// delete EpollData
							delete (EpollData *) event.data.ptr;
						}
						else
						{
							// Add EpollData to ClientInfo list
							data->before = 0;
							data->next = newInfos;
							if(newInfos != 0) newInfos->before = data;
							newInfos = data;

							/*
							// Add ClientInfo to ClientInfo list
							info->before = 0;
							info->next = newInfos;
							if(newInfos != 0) newInfos->before = info;
							newInfos = info;
							//*/
						}
					}
					catch(...)
					{
						Logger::LogException("Error in ConnectionsManager::Add(ClientInfo *)", __FILE__, __LINE__);
					}
				}

				void ConnectionsManager::Add(Listener * const listener) noexcept
				{
					try
					{
						struct epoll_event event;
						int const sock = listener->GetSocketID();

						// Make socket non-blocking
						if(MakeSocketNonBlocking(sock))
						{
							Logger::LogException("Could not make socket non blocking", __FILE__, __LINE__);
							return;
						}

						// Set epoll event to react on
						event.events = EPOLLIN | EPOLLET;
						// Set socket infos for ThreadPoolFunction
						EpollData * const data = new EpollData(listener);
						event.data.ptr = (void *) data;

						// Lock listeners list
						std::unique_lock<std::mutex> const listLock(listenerListMutex);

						// Add socket to epoll buffer
						if(epoll_ctl(clientBuffer, EPOLL_CTL_ADD, sock, &event) == -1)
						{
							// Log Error
							Logger::LogException("Could not add new listener to clientBuffer", __FILE__, __LINE__);

							// Ensure not to have the socket in epoll buffer
							epoll_ctl(clientBuffer, EPOLL_CTL_DEL, sock, 0);

							// delete EpollData
							delete (EpollData *) event.data.ptr;
						}
						else
						{
							// Add Listener to listeners list
							data->before = 0;
							data->next = listeners;
							if(listeners != 0) listeners->before = data;
							listeners = data;

							//listeners.insert(sock);
						}
					}
					catch(...)
					{
						Logger::LogException("Error in ConnectionsManager::Add(Socket *)", __FILE__, __LINE__);
					}
				}

				void ConnectionsManager::AddThreads(size_t n) noexcept
				{
					try
					{
						//Lock worker thread list and shared thread variable
						std::unique_lock<std::mutex> const listLock(workerThreadListMutex);
						std::unique_lock<std::mutex> communicationLock(communicationMutex);

						for(; n; --n)
						{
							//Add thread to worker thread list
							workerThreadList.push_back(std::thread(&ConnectionsManager::ThreadPoolFunction, this));

							//Inform worker thread about its iterator
							communicationVariable = --workerThreadList.end();
							communicationCondition.wait(communicationLock);
						}
					}
					catch(...)
					{
						Logger::LogException("Error in ConnectionsManager::AddThreads", __FILE__, __LINE__);
					}
				}

				void ConnectionsManager::DeleteAllClients() noexcept
				{
					try
					{
						std::unique_lock<std::mutex> const listLock(infoListMutex);

						//while(oldInfos != 0) DestroyInner(oldInfos);
						//while(newInfos != 0) DestroyInner(newInfos);
						while(oldInfos != 0) RemoveInner(oldInfos->clientInfo->fd, oldInfos);
						while(newInfos != 0) RemoveInner(newInfos->clientInfo->fd, newInfos);
					}
					catch (...)
					{
						Logger::LogException("Error in ConnectionsManager::DeleteAllEntries", __FILE__, __LINE__);
					}
				}

				void ConnectionsManager::DeleteAllListeners() noexcept
				{
					try
					{
						std::unique_lock<std::mutex> const listLock(listenerListMutex);

						while(listeners != 0) RemoveInner(listeners->listener->GetSocketID(), listeners);
					}
					catch(...)
					{
						Logger::LogException("Error in ConnectionsManager::DeleteAllListeners", __FILE__, __LINE__);
					}
				}

				void ConnectionsManager::DeleteOldClients() noexcept
				{
					try
					{
						std::unique_lock<std::mutex> const listLock(infoListMutex);

						while(oldInfos != 0) RemoveInner(oldInfos->clientInfo->fd, oldInfos);

//						while(oldInfos != 0)
//						{
//							DestroyInner(oldInfos);
//						}

						oldInfos = newInfos;
						newInfos = 0;
					}
					catch (...)
					{
						Logger::LogException("Error in ConnectionsManager::DeleteOldEntries", __FILE__, __LINE__);
					}
				}

				void ConnectionsManager::Remove(int const fd, EpollData const * const d) noexcept
				{
					std::unique_lock<std::mutex> const listLock(infoListMutex);

					RemoveInner(fd, d);
				}

				void ConnectionsManager::RemoveInner(int const fd, EpollData const * const d) noexcept
				{
					try
					{
						epoll_ctl(clientBuffer, EPOLL_CTL_DEL, fd, 0);

						switch(d->type)
						{
						case CLIENT_INFO:
							{
								// Remove EpollData from lists
								if(d->before != 0) d->before->next = d->next;
								else if(oldInfos == d) oldInfos = d->next;
								else if(newInfos == d) newInfos = d->next;
								else Logger::LogException("ClinetInfo in no list -> can not be removed", __FILE__, __LINE__);

								if(d->next != 0) d->next->before = d->before;

								/*
								// Remove ClientInfo from lists
								ClientInfo * const info = d->clientInfo.get();
								if(info->before != 0) info->before->next = info->next;
								else if(oldInfos == info) oldInfos = info->next;
								else if(newInfos == info) newInfos = info->next;
								else Logger::LogException("Info without before in no list", __FILE__, __LINE__);

								if(info->next != 0) info->next->before = info->before;
								*/
							}

							break;
						case LISTENER:
							if(d->before != 0) d->before->next = d->next;
							else if(listeners == d) listeners = d->next;
							else Logger::LogException("Listener in no list -> can not be removed", __FILE__, __LINE__);

							if(d->next != 0) d->next->before = d->before;
							break;
						}

						// Delete EpollData object
						delete d;
					}
					catch(...)
					{
						Logger::LogException("Unknown error in ConnectionsManager::RemoveInner", __FILE__, __LINE__);
					}
				}

				bool ConnectionsManager::MakeSocketNonBlocking(int const sock) noexcept
				{
					// Get actual flags
					int flags = fcntl(sock, F_GETFL, 0);
					if(flags == -1) return true;

					// Update flags
					flags |= O_NONBLOCK;

					// Set flags
					int const s = fcntl(sock, F_SETFL, flags);
					if(s == -1) return true;

					return false;
				}

				void ConnectionsManager::RemoveThreads(size_t n) noexcept
				{
					try
					{
						std::unique_lock<std::mutex> const listLock(workerThreadListMutex);
						std::unique_lock<std::mutex> communicationLock(communicationMutex);

						for(n = std::min(n, workerThreadList.size()); n; --n)
						{
							eventfd_write(eventSock, EVENT_SOCKET_CALL_STOP);
							//if(workerThreadList.size() > 1) eventfd_write(eventSock, EVENT_SOCKET_CALL_STOP);
							//else eventfd_write(eventSock, EVENT_SOCKET_CALL_STOP_AND_DESTROY);
							communicationCondition.wait(communicationLock);
							communicationVariable->join();
							workerThreadList.erase(communicationVariable);
						}

						if(workerThreadList.empty())
						{
							epoll_ctl(clientBuffer, EPOLL_CTL_DEL, eventSock, 0);
							Logger::LogEvent("No worker threads left");
						}
					}
					catch(...)
					{
						Logger::LogException("Error in ConnectionsManager::RemoveThreads", __FILE__, __LINE__);
					}
				}

				ConnectionsManager::~ConnectionsManager()
				{
					try
					{
						Logger::LogEvent("Stopping");

						//Stop/Remove all worker threads
						RemoveThreads(std::numeric_limits<size_t>::max());

						Logger::LogEvent("Threads removed");

						// Delete all client infos
						DeleteAllClients();

						Logger::LogEvent("All clients deleted");

						// Delete all listeners
						DeleteAllListeners();

						Logger::LogEvent("All listeners deleted");

						//close event socket
						close(eventSock);

						Logger::LogEvent("ConnectionsManager stopped");
					}
					catch(...)
					{
						Logger::LogException("Error in destructor of ConnectionsManager", __FILE__, __LINE__);
					}
				}

				void ConnectionsManager::ThreadPoolFunction() noexcept
				{
					try
					{
						bool running = true;
#ifndef DIRECT_TIMEOUT
						bool deleteOldEntriesEnabled = false;
#endif
						std::list<std::thread>::iterator myIterator;

						// Fetch worker threads own iterator for identification
						{
							std::unique_lock<std::mutex> const communicationLock(communicationMutex);

							myIterator = communicationVariable;

							// Inform creator about successful read
							communicationCondition.notify_one();
						}

						// Ensure that the database holds a connection for this thread
						std::unique_ptr<const Product> const pTData(perThreadDataFactory.CreateProduct());

						// Allocate array for epoll event data
						epoll_event * const events = (epoll_event * const) calloc(MAXEVENTS, sizeof(epoll_event));
						//epoll_event events[MAXEVENTS];

						// Declaration of loop variables
						std::shared_ptr<ClientInfo> infoData[MAXEVENTS];
						std::shared_ptr<Listener> listenerData[MAXEVENTS];
						uint32_t infoEventCalls[MAXEVENTS];
						uint32_t listenerEventCalls[MAXEVENTS];
						uint numInfoData, numListenerData;

						// While worker thread should continue running ...
						while(running)
						{
							try
							{
								// reset number of clientInfos and listeners to handle outside critical section
								numInfoData = numListenerData = 0;

								/// EPOLL CRITICAL SECTION ///
								{
									// Ensure that only one thread can manipulate epoll data at once
									std::unique_lock<std::mutex> const lock(epollMutex);

									// Wait for epoll events
									int const eventsLen = epoll_wait(clientBuffer, events, MAXEVENTS, -1);

									// for all fetched events ...
									for(int i = 0; i < eventsLen; ++i)
									{
										// Cast epoll data ptr
										EpollData * const data = (EpollData * const) events[i].data.ptr;

										if(data == 0) // If event received ...
										{
											// Acquire unique communication channel
											std::unique_lock<std::mutex> const communicationLock(communicationMutex);

											// Read from event socket
											eventfd_t val = 0;
											eventfd_read(eventSock, &val);

											// If thread should stop ...
#ifndef DIRECT_TIMEOUT
											if(val == EVENT_SOCKET_CALL_STOP)
											{
#endif
												// Set thread to do its last ("while") loop pass
												running = false;
												// Inform initiator about reception
												communicationVariable = myIterator;
												communicationCondition.notify_one();
												// Log thread termination
												Logger::LogEvent("Thread terminating");
#ifndef DIRECT_TIMEOUT
											}
											// If old clientInfos should get removed ...
											else if(val == EVENT_SOCKET_CALL_TIMEOUT) deleteOldEntriesEnabled = true;
#endif
										}
										else if((events[i].events & EPOLL_ERROR_OR_DELETE))
										{
											// Remove socket from epoll and delete its EpollData object
											if(data->type == CLIENT_INFO || data->type == LISTENER) Remove(events[i].data.fd, data);
											else Logger::LogException("Unknown epoll data type in ConnectionsManager::ThreadPoolFunction", __FILE__, __LINE__);
										}
										else if((events[i].events & EPOLLIN) || (events[i].events & EPOLLOUT))
										{
											if(data->type == CLIENT_INFO)
											{
												// Add the ClientInfo to list of those
												// to handle outside critical section
												infoData[numInfoData] = data->clientInfo;
												infoEventCalls[numInfoData] = events[i].events;
												++numInfoData;

												// Prevent timeout for that ClientInfo
												ToNew(data);
											}
											else if(data->type == LISTENER)
											{
												// Add the Listener to list of those
												// to handle outside critical section
												listenerData[numListenerData] = data->listener;
												listenerEventCalls[numListenerData] = events[i].events;
												++numListenerData;
											}
											else Logger::LogException("Unknown epoll data type in ConnectionsManager::ThreadPoolFunction", __FILE__, __LINE__);
										}
										else Logger::LogEvent("Unknown epoll event");
									}

#ifndef DIRECT_TIMEOUT
									// If old ClientInfos sould be removed (timeout) ...
									if(deleteOldEntriesEnabled)
									{
										// Delete old ClientInfos
										DeleteOldEntries();
										// Reset flag
										deleteOldEntriesEnabled = false;
									}
#endif
								} /// END OF EPOLL CRITICAL SECTION ///

								// Handle the ClientInfo events
								for(uint i = 0; i < numInfoData; ++i)
								{
									// Call handler function of the ClientInfo
									if(infoEventCalls[i] & EPOLLIN) infoData[i]->MessageReceivableCB();
									else if(infoEventCalls[i] & EPOLLOUT) infoData[i]->MessageSendableCB();

									// Reset the shared_ptr
									// (Otherwise deletion of timedout ClientInfos would not work)
									infoData[i].reset();
								}

								// Handle the EPOLLIN listener events
								for(uint i = 0; i < numListenerData; ++i)
								{
									// Accept the new client
									if(listenerEventCalls[i] & EPOLLIN)
									{
										// Accept new clients
										for(ClientInfo * info = listenerData[i]->Accept(); info != 0; info = listenerData[i]->Accept())
										{
											// If accept worked -> Add it to ClientInfo list
											if(info != 0) Add(info);
										}
									}

									// Reset the shared_ptr
									listenerData[i].reset();
								}
							}
							catch (...)
							{
								Logger::LogException("Error in endless loop of ConnectionsManager::ThreadPoolFunction", __FILE__, __LINE__);
							}
						}

						free(events);
					}
					catch (...)
					{
						Logger::LogException("Error in ConnectionsManager::ThreadPoolFunction", __FILE__, __LINE__);
					}
				}

				void ConnectionsManager::TimeOut() noexcept
				{
#ifdef DIRECT_TIMEOUT
					DeleteOldClients();
#else
					eventfd_write(eventSock, EVENT_SOCKET_CALL_TIMEOUT);
#endif
				}

				void ConnectionsManager::ToNew(EpollData * const d)
				{
					std::unique_lock<std::mutex> const listLock(infoListMutex);

					if(d->before != 0)
					{
						// Remove from old position
						d->before->next = d->next;
						if(d->next != 0) d->next->before = d->before;
						// Insert into newInfos
						d->before = 0;
						d->next = newInfos;
						if(newInfos != 0) newInfos->before = d;
						newInfos = d;
					}
					else if(oldInfos == d)
					{
						// Remove from oldInfos
						oldInfos = d->next;
						if(d->next != 0) d->next->before = 0;
						// Insert into newInfos
						d->next = newInfos;
						if(newInfos != 0) newInfos->before = d;
						newInfos = d;
					}
				}
			} // namespace Network
		} // namespace IO
	} // namespace System
} // namespace Peoplez
