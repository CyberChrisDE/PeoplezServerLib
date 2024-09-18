/**
 * Copyright 2017, 2019 Christian Geldermann
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

#ifndef PEOPLEZ_SERVICES_HTTP_HTTPCLIENTINFO_H_
#define PEOPLEZ_SERVICES_HTTP_HTTPCLIENTINFO_H_

// Local includes
#include "../../System/IO/Network/ClientInfo.hpp"
#include "Enums.hpp"
#include "HttpContext.hpp"
#include "HttpRequestHandler.hpp"

// Extern includes
#include <memory>
#include <ctime>

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			/**
			 * @brief Specific ClientInfo for http requests
			 */
			class HttpClientInfo final : public System::IO::Network::ClientInfo
			{
			public:
				/**
				 * Constructor
				 *
				 * @param fileDescriptor Copy of the file descriptor of the socket (needed for ConnectionsManager)
				 * @param pageManager PageManager that should process the request
				 * @param sender Socket for communication with client/browser
				 *
				 * @par Exception safety
				 */
				HttpClientInfo(int fileDescriptor, HttpRequestHandler & requestHandler, System::IO::Network::Socket * sender);
				virtual ClientInfo *Copy() {return new HttpClientInfo(*this);}
				virtual void MessageReceivableCB();
				virtual void MessageSendableCB();
				virtual ~HttpClientInfo() {}

			private:
				void DataReceived(size_t bytesReceived);
				void BodyReceived();
				void HeaderReceived(size_t size);
				/**
				 * Relays the request to the specific modules and writes the result into the output buffer
				 */
				void MessageReady();
				void SendInner();
				void SwitchToSend();

				HttpRequestHandler & requestHandler;
				std::shared_ptr<HttpContext> const context;
				time_t firstByte;
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_HTTPCLIENTINFO_H_
