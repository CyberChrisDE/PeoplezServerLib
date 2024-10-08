/**
 * Copyright 2017, 2019, 2020 Christian Geldermann
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
#include "ClientProvider.hpp"

// Local includes
#include "../../General/Math.hpp"
#include "../../System/Logging/Logger.hpp"

// External includes
#include <thread>

/**
 * @def TIMER_INTERVAL_SEC
 * @brief Time that elapses between the checks whether the client sessions are timed out
 */
#define TIMER_INTERVAL_SEC 1800
/**
 * @def TIMEOUT_AFTER_SEC
 * @brief Time after that a session times out
 */
#define TIMEOUT_AFTER_SEC 7200

// External namespaces
using namespace std;

namespace Peoplez
{
	// Local namespaces
	using namespace System::Logging;

	namespace Services
	{
		namespace Clients
		{
			ClientProvider::ClientProvider() : timer(bind(&ClientProvider::TimerTimeout, this), chrono::seconds(TIMER_INTERVAL_SEC))
			{
				Logger::LogEvent("ClientProvider initialized");
			}

			void ClientProvider::TimerTimeout()
			{
				try
				{
					time_t maxAlive = time(0) - TIMEOUT_AFTER_SEC; //Maximal noch gültiger Zeitstempel, mit dem Clients gültig bleiben
					unique_lock<shared_timed_mutex> lock(clientsMutex);

					for(unordered_map<uint64_t, shared_ptr<Client> >::const_iterator iter = clients.begin(); iter != clients.end(); )
					{
						if(iter->second->LastRequest < maxAlive)
						{
							uint64_t userID = iter->second->Data->ID;
							unordered_map<uint64_t, User>::iterator us = users.find(userID);
							if(us != users.end())
							{
								us->second.Remove(iter->second->SessionID);
								if(us->second.IsEmpty()) users.erase(userID);
							}
							else Logger::LogEvent("User missing for client in ClientProvider::RemoveClient");

							clients.erase(iter++);
						}
						else ++iter;
					}
				}
				catch(...)
				{
					Logger::LogException("Unknown exception in ClientProvider::TimerTimeout", __FILE__, __LINE__);
				}
			}

			uint64_t ClientProvider::GetUserID(uint64_t const sessionID)
			{
				uint64_t result = 0;

				try
				{
					shared_lock<shared_timed_mutex> lock(clientsMutex);

					unordered_map<uint64_t, shared_ptr<Client> >::const_iterator cl = clients.find(sessionID);

					if(cl != clients.end())
					{
						cl->second->Touch();
						result = cl->second->Data->ID;
					}
				}
				catch(...)
				{
					Logger::LogException("Exception catched in ClientProvider::GetUserID", __FILE__, __LINE__);
				}

				return result;
			}

			shared_ptr<Client> ClientProvider::GetClient(uint64_t const sessionID)
			{
				try
				{
					shared_lock<shared_timed_mutex> lock(clientsMutex);

					unordered_map<uint64_t, shared_ptr<Client> >::const_iterator cl = clients.find(sessionID);

					if(cl == clients.end()) return 0;
					cl->second->Touch();
					return cl->second;
				}
				catch (...)
				{
					Logger::LogException("Exception catched in ClientProvider::GetClient", __FILE__, __LINE__);
				}
				return 0;
			}

			uint64_t ClientProvider::AddClient(uint64_t const userID)
			{
				try
				{
					unique_lock<shared_timed_mutex> lock(clientsMutex);

					//Erstellen einer SessionID
					uint64_t sessionID = General::Math::Rnd64();
					while(!sessionID || clients.find(sessionID) != clients.end()) sessionID = General::Math::Rnd64();

					unordered_map<uint64_t, User>::iterator us = users.find(userID);

					if(us == users.end())
					{
						shared_ptr<Client> client(new Client(sessionID, us->second.GetClient()->Data));
						clients.insert(pair<uint64_t, shared_ptr<Client> >(sessionID, client));
						User user(sessionID, client);
						users.insert(pair<uint64_t, User>(userID, user));
					}
					else if(us->second.GetClient().get() != 0)
					{
						shared_ptr<Client> client(new Client(sessionID, us->second.GetClient()->Data));
						clients.insert(pair<uint64_t, shared_ptr<Client> >(sessionID, client));
						us->second.Add(sessionID, client);
					}
					else
					{
						Logger::LogException("User having no client in ClientProvider::AddClient", __FILE__, __LINE__);
						shared_ptr<Client> client(new Client(sessionID, us->second.GetClient()->Data));
						clients.insert(pair<uint64_t, shared_ptr<Client> >(sessionID, client));
						us->second.Add(sessionID, client);
					}
					return sessionID;
				}
				catch (...)
				{
					Logger::LogException("Exception catched in ClientProvider::AddClient", __FILE__, __LINE__);
				}
				return 0;
			}

			void ClientProvider::RemoveClient(uint64_t const sessionID)
			{
				try
				{
					unique_lock<shared_timed_mutex> const lock(clientsMutex);

					unordered_map<uint64_t, shared_ptr<Client> >::const_iterator cl = clients.find(sessionID);

					if(cl != clients.end())
					{
						uint64_t const userID = cl->second->Data->ID;
						unordered_map<uint64_t, User>::iterator us = users.find(userID);
						if(us != users.end())
						{
							us->second.Remove(sessionID);
							if(us->second.IsEmpty()) users.erase(userID);
						}
						else Logger::LogEvent("User missing for client in ClientProvider::RemoveClient");

						int erased = clients.erase(sessionID);
						if(erased < 0) Logger::LogEvent("No element erased in clients in ClientProvider::RemoveClient");
						else if(erased > 1) Logger::LogException("More than 1 element erased", __FILE__, __LINE__);
					}
					else Logger::LogEvent("No such client in ClientProvider::RemoveClient");
				}
				catch (...)
				{
					Logger::LogException("Exception catched in ClientProvider::Remove", __FILE__, __LINE__);
				}
			}

			bool ClientProvider::Contains(uint64_t const sessionID)
			{
				try
				{
					shared_lock<shared_timed_mutex> const lock(clientsMutex);

					unordered_map<uint64_t, shared_ptr<Client> >::const_iterator cl = clients.find(sessionID);

					if(cl != clients.end())
					{
						cl->second->Touch();
						return true;
					}
					else return false;
				}
				catch (...)
				{
					Logger::LogException("Exception catched in ClientProvider::Contains(pwd)", __FILE__, __LINE__);
				}

				return false;
			}

			bool ClientProvider::Contains(uint64_t const sessionID, uint64_t const userID)
			{
				try
				{
					shared_lock<shared_timed_mutex> const lock(clientsMutex);

					unordered_map<uint64_t, shared_ptr<Client> >::const_iterator cl = clients.find(sessionID);

					if(cl != clients.end())
					{
						cl->second->Touch();
						return (cl->second->Data->ID == userID);
					}
				}
				catch(...)
				{
					Logger::LogException("Exception catched in ClientProvider::Contains(pwd, userID)", __FILE__, __LINE__);
				}

				return false;
			}

			ClientProvider::~ClientProvider()
			{
				unique_lock<shared_timed_mutex> const lock(clientsMutex);

				clients.clear();
				users.clear();

				Logger::LogEvent("ClientProvider closed");
			}
		} // namespace Clients
	} // namespace Services
} // namespace Peoplez

