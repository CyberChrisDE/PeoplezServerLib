/**
 * Copyright 2017, 2024 Christian Geldermann
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

#ifndef PEOPLEZ_SYSTEM_IO_NETWORK_SOCKET_HPP_
#define PEOPLEZ_SYSTEM_IO_NETWORK_SOCKET_HPP_

// Extern includes
#include <cstddef>

namespace Peoplez
{
	namespace System
	{
		namespace IO
		{
			namespace Network
			{
				/**
				 * @brief Class for sending the http response
				 * @details Class that cares of sending data and destroying the socket. Childs can reimplement the methods for adding security issues or other implementation details.
				 */
				class Socket
				{
					/*@
					predicate valid(int fd, bool is_open) = this->sock |-> fd &*& this->isOpen |-> is_open;
					@*/
				public:
					/**
					 * Constructor
					 *
					 * Initializes the object with the given socket
					 *
					 * @param so Socket to be sent to
					 */
					Socket(int const so) noexcept : isOpen(true), sock(so)
					//@ requires true;
					//@ ensures valid(so, true) &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);
					{
					}

					Socket(Socket && other) noexcept;
					//@ requires other.isOpen |-> ?is_open &*& other.sock |-> ?sock;
					//@ ensures valid(sock, is_open) &*& other.isOpen |-> false &*& other.sock |-> sock &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);

					/**
					 * Getter for the socket file descriptor
					 *
					 * @return Socket file descriptor
					 */
					inline int GetSocketID() const noexcept
					//@ requires valid(?sock, ?is_open);
					//@ ensures valid(sock, is_open) &*& result == sock;
					{return sock;}

					/**
					 * Checks whether the socket is open
					 *
					 * @return True: Socket is open; False: Socket is closed
					 */
					inline bool IsOpen() const noexcept
					//@ requires valid(?sock, ?is_open);
					//@ ensures valid(sock, is_open) &*& result == is_open;
					{return isOpen;}

					/**
					 * Receives data and writes them to the given array
					 *
					 * @param buf Buffer to write received data in
					 * @param len Size of the buffer
					 */
					virtual int Recv(char *buf, size_t len) noexcept;
					//@ requires valid(?sock, ?is_open) &*& chars(buf, len, _) &*& len <= INT_MAX &*& Peoplez::System::IO::Network::Socket_vtype(this, ?thisType);
					//@ ensures valid(sock, is_open) &*& chars(buf, len, _) &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);

					/**
					 * Sends the given data to the client
					 *
					 * @param buff Data to be send
					 * @param len Size of data in byte
					 */
					virtual int Send(const char* buf, size_t len) noexcept;
					//@ requires valid(?sock, ?is_open) &*& chars(buf, len, _) &*& len <= INT_MAX &*& Peoplez::System::IO::Network::Socket_vtype(this, ?thisType);
					//@ ensures valid(sock, is_open) &*& chars(buf, len, _) &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);

					/**
					 * Closes the socket
					 */
					virtual void Close() noexcept;
					//@ requires valid(?sock, _) &*& Peoplez::System::IO::Network::Socket_vtype(this, ?thisType);
					//@ ensures valid(sock, false) &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);

					/**
					 * Destructor
					 */
					virtual ~Socket() noexcept
					//@ requires valid(?sock, _) &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);
					//@ ensures true;
					{
						Close();
					}
				private:
					Socket() = delete;
					Socket(Socket const &other) = delete;
					Socket operator=(Socket const &rhs) = delete;

					bool isOpen;
					int sock;
				};
			} // namespace Network
		} // namespace IO
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_IO_NETWORK_SOCKET_HPP_
