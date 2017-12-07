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

#ifndef SRC_PEOPLEZ_SERVICES_HTTP_HTTPSLISTENERSNI_HPP_
#define SRC_PEOPLEZ_SERVICES_HTTP_HTTPSLISTENERSNI_HPP_

#include "../../System/IO/Network/Listener.hpp"
#include "HttpRequestHandler.hpp"
#include "../../String/PeoplezString.hpp"

#include <vector>
#include <memory>

extern "C"
{
#include <openssl/ssl.h>
}

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			struct HttpsCertificate
			{
				inline HttpsCertificate(char const * const pubKey_, char const * const privKey_,
						char const * const cipherList_ = 0)
						: pubKey(pubKey_), privKey(privKey_), cipherList(cipherList_) {}

				char const * pubKey;
				char const * privKey;
				char const * cipherList;
			};

			struct DomainToCert
			{
				DomainToCert(String::PeoplezString const domainName_, size_t const certIndex_) : domainName(domainName_), certIndex(certIndex_) {}

				String::PeoplezString domainName;
				size_t certIndex;
			};

			struct ServernameCBResult
			{
				ServernameCBResult(size_t const index_, bool const error_ = false) : index(index_), error(error_) {}

				size_t index;
				bool error;
			};

			class HttpsListenerSNI : public System::IO::Network::Listener
			{
			private:
				struct Data final
				{
					Data(std::vector<HttpsCertificate> const certs_, size_t (*cb_)(std::vector<HttpsCertificate> const &certs, char const * hostName), void * cbData_)
							: certs(certs_), cb(cb_), cbData(cbData_) {}
					Data(std::vector<DomainToCert> const domains_)
							: domains(domains_), cb(0), cbData(0) {}
					~Data(){}

					union
					{
						std::vector<HttpsCertificate> certs;
						std::vector<DomainToCert> domains;
					};

					std::vector<SSL_CTX *> contexts;

					size_t (*cb)(std::vector<HttpsCertificate> const &certs, char const * hostName);
					void * cbData;
				};

			public:
				HttpsListenerSNI(uint16_t port, HttpRequestHandler & rH, std::vector<HttpsCertificate> certs,
						size_t (*cb)(std::vector<HttpsCertificate> const &certs, char const * hostName), void * cbData = 0);
				HttpsListenerSNI(uint16_t port, HttpRequestHandler & rH, std::vector<HttpsCertificate> certs,
						std::vector<DomainToCert> domains);
				virtual ~HttpsListenerSNI();

				virtual System::IO::Network::ClientInfo * Accept();

			private:
				static int ServerNameCB(SSL * ssl, int * a, void * content);
				void Construct(uint16_t port, HttpRequestHandler & rH, std::vector<HttpsCertificate> certs);

			private:
				HttpRequestHandler & requestHandler;
				Data data;
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // SRC_PEOPLEZ_SERVICES_HTTP_HTTPSLISTENERSNI_HPP_
