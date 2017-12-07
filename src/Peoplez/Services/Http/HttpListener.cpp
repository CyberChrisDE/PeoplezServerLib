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

#include "HttpListener.hpp"

// Local includes
#include "HttpClientInfo.hpp"

// Extern includes
extern "C"
{
#include <netinet/in.h>
#include <sys/socket.h>
}

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			HttpListener::HttpListener(uint16_t const port, HttpRequestHandler & rH)
				: Listener(socket(AF_INET, (int)SOCK_STREAM, IPPROTO_TCP)), requestHandler(rH)
			{
				// Ensure that the socket is created
				if(sock < 0) throw std::runtime_error("The socket could not be created");

				{
					// Make port reusable
					int sockoptData = 1;
					setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockoptData, sizeof(sockoptData));
				}

				sockaddr_in serverAddrHttp;
				bzero(&serverAddrHttp, sizeof(serverAddrHttp));
				serverAddrHttp.sin_family = AF_INET;
				serverAddrHttp.sin_addr.s_addr = htonl(INADDR_ANY);
				serverAddrHttp.sin_port = htons(port);

				if(!bind(sock, &((sockaddr &)serverAddrHttp), sizeof(serverAddrHttp)))
				{
					if(listen(sock, 5) != 0)
					{
						close(sock);
						throw std::runtime_error("Can noch listen on that socket");
					}
				}
				else
				{
					close(sock);
					throw std::runtime_error("Can not bind socket to address");
				}
			}

			HttpListener::~HttpListener()
			{
				if(sock >= 0) close(sock);
			}

			ClientInfo * HttpListener::Accept()
			{
				int const fd = accept(sock, (sockaddr *) NULL, NULL);

				if(fd >= 0) return new HttpClientInfo(fd, requestHandler, /*clientProvider, */new Socket(fd));
				else return 0;
			}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
