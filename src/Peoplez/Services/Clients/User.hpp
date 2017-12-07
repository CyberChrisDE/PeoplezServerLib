/**
 * Copyright 2017 Christian Geldermann
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

#ifndef USER_H_
#define USER_H_

// Local includes
#include "Client.hpp"

// External includes
#include <map>

namespace Peoplez
{
	namespace Clients
	{
		class ClientProvider;
		/**
		 * @brief Shared Pointer for UserData
		 * @details Container for UserData that cares of getting erased when only one instance of it is left
		 */
		class User final
		{
		public:
			User() {}
			/**
			 * Recommended constructor
			 *
			 * @param sessionID ID of the client/session
			 * @param client Shared pinter to the client object to link to
			 */
			User(uint64_t sessionID, std::shared_ptr<Client> client);
			~User() {}

			/**
			 * Adds a client to the user
			 *
			 * @param sessionID ID of the client/session
			 * @param client Shared pointer to the client object to link to
			 */
			void Add(uint64_t sessionID, std::shared_ptr<Client> client);
			/**
			 * Indicates whether there are clients left for this user
			 *
			 * @return True: No clients left; Talse: Clients left
			 */
			bool IsEmpty() {return clients.empty();}
			/**
			 * @brief Getter for the client object
			 */
			std::shared_ptr<Client> GetClient();
			/**
			 * Removes a client/session from the user
			 */
			void Remove(uint64_t sessionID);
		private:
			std::map<uint64_t, std::shared_ptr<Client> > clients;
		};
	} // namespace Clients
} // namespace Peoplez

#endif // USER_H_
