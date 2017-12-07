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

#ifndef PEOPLEZ_SERVICES_HTTP_HTTPSLISTENER_HPP_
#define PEOPLEZ_SERVICES_HTTP_HTTPSLISTENER_HPP_

#include "../../System/IO/Network/Listener.hpp"

// Local includes
#include "HttpRequestHandler.hpp"

#include <cstdint>
#include <vector>

extern "C"
{
#include <openssl/ssl.h>
}

namespace Peoplez
{
	using namespace System::IO::Network;

	namespace Services
	{
		namespace Http
		{
			class HttpsListener: public System::IO::Network::Listener
			{
			public:
				HttpsListener(uint16_t port, HttpRequestHandler & rH, char const * puKey, char const * prKey);
				HttpsListener(uint16_t port, HttpRequestHandler & rH, std::vector<char const *> puKeys, std::vector<char const *> prKeys, std::vector<char const *> hostNames);
				virtual ~HttpsListener();

				virtual ClientInfo * Accept();
			private:
				HttpRequestHandler & requestHandler;
				SSL_CTX * ctx = 0;
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_HTTPSLISTENER_HPP_
