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

// Own headers
#include "Socket.hpp"

// Extern includes
extern "C"
{
#include <unistd.h>
#include <sys/socket.h>
}

namespace Peoplez
{
	namespace System
	{
		namespace IO
		{
			namespace Network
			{
				Socket::Socket(Socket && other) noexcept : open(other.open), sock(other.sock)
				{
					other.open = false;
				}

				int Socket::Recv(char * const buf, size_t const len) noexcept
				{
					return recv(sock, buf, len, 0);
				}

				int Socket::Send(char const * const buff, int const len) noexcept
				{
					return send(sock, buff, len, 0);
				}

				void Socket::Close() noexcept
				{
					if(open)
					{
						close(sock);
						open = false;
					}
				}
			} // namespace Network
		} // namespace IO
	} // namespace System
} // namespace Peoplez
