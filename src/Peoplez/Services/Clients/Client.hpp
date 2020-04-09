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

#ifndef PEOPLEZ_SERVICES_CLIENTS_CLIENT_H_
#define PEOPLEZ_SERVICES_CLIENTS_CLIENT_H_

// Local includes
#include "UserData.hpp"

// External includes
#include <memory>
#include <stdexcept>

extern "C"
{
#include <openssl/rand.h>
}

namespace Peoplez
{
	namespace Services
	{
		namespace Clients
		{
			/**
			 * @brief Container for client information.
			 */
			class Client final
			{
			public:
				/**
				 * Constructor
				 *
				 * @param sessionID Id of the client/session
				 * @param userData Data of the clients user
				 */
				Client(uint64_t sessionID, std::shared_ptr<UserData> userData) : LastRequest(time(0)), SessionID(sessionID), Data(userData)
				{
					int rc = RAND_bytes(SessionSecret, sizeof(SessionSecret));

					if(rc != 1) throw std::runtime_error("Could not generate cryptographically secure session secret.");
				}
				/**
				 * Sets LastRequst to the actual time
				 */
				inline void Touch() noexcept {LastRequest = time(0);}

				/**
				 * Time of last request of the client
				 *
				 * Time of the last call of Touch()
				 */
				time_t LastRequest;
				/**
				 * SessionID of the client
				 */
				uint64_t SessionID;
				/**
				 * Session secret of the client
				 */
				unsigned char SessionSecret[32];
				/**
				 * User as which the client is logged in
				 */
				std::shared_ptr<UserData> Data;
			};
		} // namespace Clients
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_CLIENTS_CLIENT_H_
