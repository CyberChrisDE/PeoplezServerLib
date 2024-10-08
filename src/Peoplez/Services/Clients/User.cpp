/**
 * Copyright 2017, 2020 Christian Geldermann
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
#include "User.hpp"

using namespace std;

namespace Peoplez
{
	namespace Services
	{
		namespace Clients
		{
			User::User(uint64_t const sessionID, shared_ptr<Client> const client)
			{
				clients.insert(std::pair<uint64_t, shared_ptr<Client> >(sessionID, client));
			}

			void User::Add(uint64_t const sessionID, shared_ptr<Client> const client)
			{
				clients.insert(std::pair<uint64_t, shared_ptr<Client> >(sessionID, client));
			}

			shared_ptr<Client> User::GetClient() const
			{
				if(clients.empty()) return shared_ptr<Client>(0);
				else return clients.begin()->second;
			}

			shared_ptr<Client> User::GetClient(uint64_t clientID) const
			{
				map<uint64_t, std::shared_ptr<Client> >::const_iterator iter = clients.find(clientID);
				if(iter != clients.end()) return iter->second;
				else return 0;
			}

			void User::Remove(uint64_t const sessionID)
			{
				clients.erase(sessionID);
			}
		} // namespace Clients
	} // namespace Services
} // namespace Peoplez
