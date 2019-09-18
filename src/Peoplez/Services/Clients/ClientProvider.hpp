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

#ifndef PEOPLEZ_SERVICES_CLIENTS_CLIENTPROVIDER_H_
#define PEOPLEZ_SERVICES_CLIENTS_CLIENTPROVIDER_H_

// Local includes
#include "../../System/Timer.hpp"
#include "Client.hpp"
#include "User.hpp"

// Extern include branchers
//#define UNORDERED

// Extern includes
#ifdef UNORDERED
#include <boost/unordered_map.hpp>
#else
#include <map>
#endif

//#include <boost/thread/shared_mutex.hpp>
#include <shared_mutex>

namespace Peoplez
{
	namespace Services
	{
		namespace Clients
		{
			class IClientProvider
			{
			public:
				virtual std::shared_ptr<Client> GetClient(uint64_t sessionID) {return 0;}
				virtual ~IClientProvider() {}
			};

			/**
			 * @brief Manages all client and user data
			 */
			class ClientProvider final : public IClientProvider
			{
			public:
				/**
				 * Constructor
				 *
				 * @param database Database object that should be used
				 */
				ClientProvider();
				virtual ~ClientProvider();

				/**
				 * Handler for Timer(), that checks all clients whether their sessions are timed out
				 */
				void TimerTimeout();
				/**
				 * Getter for the userID matching a specific sessionID
				 *
				 * @param sessionID SessionID of the user
				 *
				 * @return UserUD of the user with the given sessionID
				 */
				uint64_t GetUserID(uint64_t sessionID);
				/**
				 * Getter for the client with the given sessionID
				 *
				 * @param sessionID SessionID of the user
				 *
				 * @return Client with the given sessionID
				 */
				virtual std::shared_ptr<Client> GetClient(uint64_t sessionID);
				/**
				 * Adds a new client
				 *
				 * @param userID ID of the clients user
				 *
				 * @return SessionID for the client
				 */
				uint64_t AddClient(uint64_t userID);
				/**
				 * Removes the Client with the given sessionID
				 *
				 * @param sessionID SessionID of the client
				 */
				void RemoveClient(uint64_t sessionID);
				/**
				 * Checks whether a client with the given sessionID is logged on
				 *
				 * @param sessionID SessionID of the client
				 *
				 * @return True if such a client is logged on, false otherwise
				 */
				bool Contains(uint64_t sessionID);
				/**
				 * Checks whether a client with the given userID and sessionID is logged on
				 *
				 * @param sessionID SessionID of the client
				 * @param userID ID of the user
				 *
				 * @return True if such a client is logged on, false otherwise
				 */
				bool Contains(uint64_t sessionID, uint64_t userID);

			private:
				std::shared_ptr<UserData> FetchUserData(uint64_t userID);

			#ifdef UNORDERED
				boost::unordered_map<uint64_t, std::shared_ptr<Client>> clients;
				boost::unordered_map<uint64_t, User> users;
			#else
				std::map<uint64_t, std::shared_ptr<Client> > clients;
				std::map<uint64_t, std::shared_ptr<User> > users;
			#endif
				std::shared_timed_mutex clientsMutex;
				System::Timer timer;
			};
		} // namespace Clients
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_CLIENTS_CLIENTPROVIDER_H_
