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

#include "HttpsListener.hpp"

#include "HttpClientInfo.hpp"
#include "../../System/IO/Network/SecureSocket.hpp"

#include <iostream>

extern "C"
{
#include <arpa/inet.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <unistd.h>
}

using namespace std;

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			HttpsListener::HttpsListener(uint16_t const port, HttpRequestHandler & rH, char const * const puKey, char const * const prKey)
				: Listener(0), requestHandler(rH), ctx(0)
			{
				//TODO: Handle exceptions

				// Init Openssl
				SSL_load_error_strings();
				OpenSSL_add_ssl_algorithms();

				// Create Context
				{
					SSL_METHOD const * const method = SSLv23_server_method();
					if(!method) throw std::runtime_error("Unable to create SSL method");

					ctx = SSL_CTX_new(method);
					if(!ctx) throw std::runtime_error("Unable to create SSL context");
				}

				// configure_context
				{
					SSL_CTX_set_ecdh_auto(ctx, 1);

					// Set the key and cert
					if(SSL_CTX_use_certificate_file(ctx, puKey, SSL_FILETYPE_PEM) <= 0) throw std::runtime_error("Unable to load cert file");
					if(SSL_CTX_use_PrivateKey_file(ctx, prKey, SSL_FILETYPE_PEM) <= 0) throw std::runtime_error("Unable to load private key file");

					if(SSL_CTX_set_cipher_list(ctx, "kEECDH kEDH +ECDH +AES128 +SHA !aNULL !eNULL !LOW !3DES !MD5 !EXP !DSS !PSK !SRP !kECDH !CAMELLIA !IDEA !SEED !RC4") <= 0)
						throw std::runtime_error("Unable to set cipher suite");
				}

				// Create Socket
				{
					sockaddr_in addr;
					addr.sin_family = AF_INET;
					addr.sin_port = htons(port);
					addr.sin_addr.s_addr = htonl(INADDR_ANY);

					sock = socket(AF_INET, SOCK_STREAM, 0);

					{
						// Make port reusable
						int const sockoptData = 1;
						setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockoptData, sizeof(sockoptData));
					}

					if(sock < 0) throw std::runtime_error("Unable to create socket");
					if(bind(sock, (sockaddr *) &addr, sizeof(addr)) < 0) throw std::runtime_error("Unable to bind");
					if(listen(sock, 1) < 0) throw std::runtime_error("Unable to listen");
				}
			}

			HttpsListener::~HttpsListener()
			{
				if(sock > 0) close(sock);
				SSL_CTX_free(ctx);

				EVP_cleanup();
			}

			ClientInfo * HttpsListener::Accept()
			{
				int const client = accept(sock, 0, 0);

				if(client >= 0)
				{
					SSL * const ssl = SSL_new(ctx);
					SSL_set_fd(ssl, client);

					if(SSL_accept(ssl) <= 0) return 0;
					else return new HttpClientInfo(client, requestHandler, new SecureSocket(client, ssl));
				}
				else return 0;
			}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
