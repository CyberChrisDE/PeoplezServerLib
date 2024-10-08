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

#ifndef PEOPLEZ_SYSTEM_IO_NETWORK_SECURESOCKET_HPP_
#define PEOPLEZ_SYSTEM_IO_NETWORK_SECURESOCKET_HPP_

// Local includes
#include "Socket.hpp"

// Extern includes
//#include <type_traits>
//extern "C"
//{
#include <openssl/ssl.h>
//}

namespace Peoplez
{
	namespace System
	{
		namespace IO
		{
			namespace Network
			{
				/**
				 * @brief Class for sending the https response
				 * @details Class theat takes care of sending data per ssl and destroying the complete ssl socket.
				 */
				class SecureSocket final : public Socket
				{
					/*@
					predicate valid(int fd, bool is_open) = this->valid(&typeid(Peoplez::System::IO::Network::Socket))(fd, is_open) &*& Peoplez::System::IO::Network::SecureSocket_bases_constructed(this) &*& this->ssl |-> ?s &*& (is_open == true ? SSL_Obj(s) : true);
					@*/
				public:
					/**
					 * Constructor
					 *
					 * @param so Socket to be sent to
					 * @param s Pointer to the ssl handler that should encode the response
					 */
					SecureSocket(int so, SSL* s) /*noexcept(std::is_nothrow_constructible<Socket>::value)*/ noexcept : Socket(so), ssl(s)
					//@ requires SSL_Obj(s);
					//@ ensures valid(so, true) &*& Peoplez::System::IO::Network::SecureSocket_vtype(this, thisType);
					{
						//@ close valid(so, true);
					}
					/**
					 * Receives data from the client using ssl
					 */
					virtual int Recv(char *buf, size_t len) noexcept;
					//@ requires valid(?sock, ?is_open) &*& chars(buf, len, _) &*& len <= INT_MAX &*& Peoplez::System::IO::Network::Socket_vtype(this, ?thisType);
					//@ ensures valid(sock, is_open) &*& chars(buf, len, _) &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);
					/**
					 * Sends the given data to the client using ssl
					 *
					 * @param buff Data to be send
					 * @param len Size of data in byte
					 */
					virtual int Send(char const * buf, size_t len) noexcept;
					//@ requires valid(?sock, ?is_open) &*& chars(buf, len, _) &*& len <= INT_MAX &*& Peoplez::System::IO::Network::Socket_vtype(this, ?thisType);
					//@ ensures valid(sock, is_open) &*& chars(buf, len, _) &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);
					/**
					 * Closes the complete socket
					 *
					 * Closes the ssl socket handler and the socket itself
					 */
					virtual void Close() noexcept;
					//@ requires valid(?sock, _) &*& Peoplez::System::IO::Network::Socket_vtype(this, ?thisType);
					//@ ensures valid(sock, false) &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);
					/**
					 * Destructor
					 */
					virtual ~SecureSocket() noexcept
					//@ requires valid(?sock, _) &*& Peoplez::System::IO::Network::Socket_vtype(this, thisType);
					//@ ensures true;
					{Close();}
				private:
					SecureSocket() = delete;
					SecureSocket(SecureSocket const &other) = delete;
					SecureSocket operator=(SecureSocket const &rhs) = delete;

					SSL * ssl;
				};
			} // namespace Network
		} // namespace IO
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_IO_NETWORK_SECURESOCKET_HPP_
