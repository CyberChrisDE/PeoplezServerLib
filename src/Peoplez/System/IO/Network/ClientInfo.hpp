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

#ifndef PEOPLEZ_SYSTEM_IO_NETWORK_CLIENTINFO_HPP_
#define PEOPLEZ_SYSTEM_IO_NETWORK_CLIENTINFO_HPP_

namespace Peoplez
{
	namespace System
	{
		namespace IO
		{
			namespace Network
			{
				/**
				 * @brief Holds all information about a client connection/socket
				 */
				class ClientInfo
				{
				public:
					/**
					 * Constructor
					 *
					 * @param sock socket descriptor
					 */
					ClientInfo(int const sock = 0) noexcept : fd(sock) {}
					/**
					 * Copy constructor
					 *
					 * @param other The instance to copy from
					 */
					ClientInfo(ClientInfo const & other) noexcept : fd(other.fd) {}
					/**
					 * Creates a copy of the object
					 *
					 * @return Pointer to copy of the object itself
					 */
					virtual ClientInfo *Copy() = 0;
					/**
					 * Handler for receivable data events
					 *
					 * When more data from the client/browser can be read this handler is called
					 */
					virtual void MessageReceivableCB() = 0;
					/**
					 * Handler for sendable data events
					 *
					 * When more data can be sent to the client/browser this handler is called
					 */
					virtual void MessageSendableCB() = 0;
					/**
					 * Destructor
					 */
					virtual ~ClientInfo() noexcept {}
					/**
					 * @brief file descriptor for connection to client
					 */
					int const fd;
				};
			} // namespace Network
		} // namespace IO
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_IO_NETWORK_CLIENTINFO_HPP_
