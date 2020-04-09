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

// Local includes
#include "HttpsListenerSNI.hpp"
#include "HttpClientInfo.hpp"
#include "../../System/IO/Network/SecureSocket.hpp"

// External includes
#include <cstdint>

extern "C"
{
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
}

#ifndef PEOPLEZ_DEFAULT_CIPHER_LIST
	#define PEOPLEZ_DEFAULT_CIPHER_LIST "kEECDH kEDH +ECDH +AES128 +SHA !aNULL !eNULL !LOW !3DES !MD5 !EXP !DSS !PSK !SRP !kECDH !CAMELLIA !IDEA !SEED !RC4"
#endif

using namespace Peoplez::System::IO::Network;

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			int HttpsListenerSNI::ServerNameCB(SSL * ssl, int * a, void * content)
			{
				// Fetch necessary data
				Data * const cbData = static_cast<Data *>(content);
				char const * const servername = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
				size_t snLen = strlen(servername);

				size_t ctx = 0; // The context to be used

				if(cbData->cb)
				{
					// Call callback function
					ServernameCBResult const cbResult = cbData->cb(cbData->certs, servername);

					// Handle callback result
					if(cbResult.error) return SSL_TLSEXT_ERR_NOACK;

					ctx = cbResult.index;
				}
				else
				{
					// Search for correct domain name
					size_t i = 0;

					for(; i < cbData->domains.size(); ++i)
					{
						if(cbData->domains[i].domainName.EqualTo(servername, snLen))
						{
							ctx = i;
							break;
						}
					}

					// Return error if domain name not found
					if(i >= cbData->domains.size()) return SSL_TLSEXT_ERR_NOACK;
				}

				// Keep safe if ctx is invalid
				if(ctx >= cbData->contexts.size()) return SSL_TLSEXT_ERR_NOACK;

				// Set context
				SSL_set_SSL_CTX(ssl, cbData->contexts[ctx]);

				// return OK
				return SSL_TLSEXT_ERR_OK;
			}

			void HttpsListenerSNI::Construct(uint16_t port, std::vector<HttpsCertificate> certs)
			{
				//TODO Mögliche Fehler im Logger ausgeben
				//TODO Behandeln: cb == NULL

				data.contexts.reserve(certs.size());

				// init_openssl
				SSL_load_error_strings();
				OpenSSL_add_ssl_algorithms();

				// Create Context
				{
					SSL_METHOD const * const method = SSLv23_server_method();
					if(!method) throw std::runtime_error("Unable to create SSL method");

					bool error = false;

					try
					{
						for(size_t i = 0; i < certs.size(); ++i)
						{
							// Create context
							SSL_CTX * const ctx = SSL_CTX_new(method);

							if(!ctx)
							{
								error = true;
								break;
							}

							// Configure context
							bool configError = false;

							SSL_CTX_set_ecdh_auto(ctx, 1);
							if(SSL_CTX_use_certificate_file(ctx, certs[i].pubKey, SSL_FILETYPE_PEM) <= 0) configError = true;
							if(!configError && SSL_CTX_use_PrivateKey_file(ctx, certs[i].privKey, SSL_FILETYPE_PEM) <= 0) configError = true;

							char const * const cipherList = certs[i].cipherList ? certs[i].cipherList : PEOPLEZ_DEFAULT_CIPHER_LIST;
							if(!configError && SSL_CTX_set_cipher_list(ctx, cipherList) <= 0) configError = true;

							if(!configError)
							{
								// Set callback function
								SSL_CTX_set_tlsext_servername_callback(ctx, ServerNameCB);

								// Set context specific content passed to callback
								SSL_CTX_set_tlsext_servername_arg(ctx, &data);
							}

							// Add context to m_contexts
							data.contexts.push_back(configError ? 0 : ctx);
						}
					}
					catch(...) {error = true;}

					// Create socket
					if(!error)
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

						if(sock < 0) error = true;
						if(bind(sock, (sockaddr *) &addr, sizeof(addr)) < 0) error = true;
						if(listen(sock, 1) < 0) error = true;
					}

					// Destroy socket and contexts if error occured
					if(error)
					{
						if(sock > 0) close(sock);

						for(size_t i = 0; i < data.contexts.size(); ++i)
						{
							if(data.contexts[i]) SSL_CTX_free(data.contexts[i]);
						}

						data.contexts.clear();
					}
				}
			}

			HttpsListenerSNI::HttpsListenerSNI(uint16_t const port, HttpRequestHandler & rH, std::vector<HttpsCertificate> const certs, size_t (*cb)(std::vector<HttpsCertificate> const &certs, char const * hostName), void * cbData) : requestHandler(rH), data(certs, cb, cbData)
			{
				Construct(port, certs);
			}

			HttpsListenerSNI::HttpsListenerSNI(uint16_t port, HttpRequestHandler & rH, std::vector<HttpsCertificate> certs, std::vector<DomainToCert> domains) : requestHandler(rH), data(domains)
			{
				Construct(port, certs);
			}

			HttpsListenerSNI::~HttpsListenerSNI()
			{
				// Close socket
				if(sock > 0) close(sock);

				// Destroy SSL contexts
				for(size_t i = 0; i < data.contexts.size(); ++i)
				{
					if(data.contexts[i]) SSL_CTX_free(data.contexts[i]);
				}

				// ???
				EVP_cleanup();
			}

			ClientInfo * HttpsListenerSNI::Accept()
			{
				int const client = accept(sock, 0, 0);

				if(client >= 0 && !data.contexts.empty())
				{
					SSL * const ssl = SSL_new(data.contexts[0]);
					SSL_set_fd(ssl, client);

					if(SSL_accept(ssl) <= 0) return 0;
					else return new HttpClientInfo(client, requestHandler, new SecureSocket(client, ssl));
					return 0;
				}
				else return 0;
			}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
