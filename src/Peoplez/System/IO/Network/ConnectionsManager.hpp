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

#ifndef PEOPLEZ_SYSTEM_IO_NETWORK_CONNECTIONSMANAGER_HPP_
#define PEOPLEZ_SYSTEM_IO_NETWORK_CONNECTIONSMANAGER_HPP_

// Local includes
#include "../../../General/Patterns/Factory.hpp"
#include "../../Timer.hpp"
#include "Listener.hpp"
#include "ClientInfo.hpp"

// Extern includes
#include <thread>
#include <list>

namespace Peoplez
{
	namespace System
	{
		namespace IO
		{
			namespace Network
			{
				struct EpollData;

				/**
				 * @brief Manages all client connections
				 * @details Manages all connections that are needed for reading and/or writing. Does not manage listening sockets
				 */
				class ConnectionsManager final
				{
				public:
					/**
					 * Constructor
					 *
					 * Initializes epoll and the event socket and starts the worker threads
					 *
					 * @param database Database to reserve a connection per thread at
					 * @param threads Number of worker threads to use
					 */
					ConnectionsManager(General::Patterns::Factory &perTreadFactory, size_t threads);
					/**
					 * Stops the ConnectionsManager
					 *
					 * stops all worker threads and cleans up everything in the ConnectionsManager
					 *
					 * @par Exception safety
					 *  No-throw guarantee
					 */
					~ConnectionsManager();

					/**
					 * Adds a new ClientInfo to wait for events on its socket
					 *
					 * @param info ClientInfo including all information about the connection
					 */
					void Add(ClientInfo *info) noexcept;
					/**
					 * Adds a new Listener to wait for new connections
					 */
					void Add(Listener * listener) noexcept;
					/**
					 * Adds n threads to the thread pool
					 *
					 * @param n Number of threads to add
					 */
					void AddThreads(size_t n) noexcept;
					/**
					 * Makes a socket non blocking
					 *
					 * @param sock Socket to make non blocking
					 *
					 * @return Indicates whether the operation was successfull
					 */
					static bool MakeSocketNonBlocking(int sock) noexcept;
					/**
					 * Removes n threads from the thread pool
					 *
					 * @param n Number of threads to remove
					 */
					void RemoveThreads(size_t n) noexcept;

				private:
					enum EventSocketCall
					{
						EVENT_SOCKET_CALL_STOP = 1,
						EVENT_SOCKET_CALL_TIMEOUT = 2
					};

					//Forbid copies
					ConnectionsManager(ConnectionsManager const &) = delete;
					ConnectionsManager & operator=(ConnectionsManager const &) = delete;
					ConnectionsManager & operator=(ConnectionsManager &&) = delete;

					void DeleteAllClients() noexcept;
					void DeleteAllListeners() noexcept;
					void DeleteOldClients() noexcept;
					void Remove(int fd, EpollData const * d) noexcept;
					void RemoveInner(int fd, EpollData const * d) noexcept;
					void AddEventSock() noexcept;
					void ThreadPoolFunction() noexcept;
					void ThreadPoolFunction2() noexcept;
					void TimeOut() noexcept;
					void ToNew(EpollData * d);

					/**
					 * @brief epoll file descriptor
					 */
					int const clientBuffer;
					std::mutex epollMutex;
					int const eventSock;
					std::mutex infoListMutex;
					std::mutex listenerListMutex;
					EpollData * listeners;
					EpollData * newInfos;
					EpollData * oldInfos;
					std::list<std::thread>::iterator communicationVariable;
					std::mutex communicationMutex;
					std::condition_variable communicationCondition;
					//General::TimerMember<ConnectionsManager> timer;
					General::Patterns::Factory &perThreadDataFactory;
					System::Timer const timer;
					std::list<std::thread> workerThreadList;
					std::mutex workerThreadListMutex;

					static const uint32_t EPOLL_ERROR_OR_DELETE;
				};
			} // namespace Network
		} // namespace IO
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_IO_NETWORK_CONNECTIONSMANAGER_HPP_
