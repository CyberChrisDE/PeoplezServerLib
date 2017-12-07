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

#ifndef PEOPLEZ_SERVICES_HTTP_HTTPCONTEXT_H_
#define PEOPLEZ_SERVICES_HTTP_HTTPCONTEXT_H_

// Local includes
#include "../../System/IO/Network/Socket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "PostParam.hpp"

// Extern includes
#include <boost/thread/mutex.hpp>

namespace Peoplez
{
	namespace Services
	{
		 namespace Http
		 {
		 	 enum HttpRequestReadStatus
			 {
		 		 READ_STATUS_COMPLETE,
				 READ_STATUS_HEADERS,
				 READ_STATUS_BODY
			 };

		 	 class RequestStream
			 {
		 	 public:
		 		 HttpRequestReadStatus status = READ_STATUS_COMPLETE;
		 		 String::PeoplezString input;
			 };

			/**
			 * @brief Container for request and response
			 */
			class HttpContext final
			{
			public:
				/**
				 * Constructor
				 *
				 * @param s Socket for sending the response to the client/browser
				 */
				HttpContext(System::IO::Network::Socket *s) : request(), response(), InputBuffer(), OutputBuffer(), Status(HTTP_SOCKET_STATUS_RECEIVE_HEADER), SendableCBEnabled(false), sender(s) {}
				/**
				 * Extracts all information from the http header
				 *
				 * @return Indicates whether the header is ok
				 *
				 * @par Exception safety
				 *  No-throw guarantee
				 */
				HttpStatusCode HandleHeader() noexcept;
				//HttpStatusCode HandleRequestStream(RequestStream & readInfo) noexcept;
				/**
				 * Extracts the information from the http body
				 *
				 * @par Exception safety
				 *  No-throw guarantee
				 */
				HttpStatusCode HandleBody(size_t size) noexcept;
				/**
				 * Extracts the information from a MultipartFormData body
				 *
				 * @param src MultipartFormData in binary form
				 * @param dest Target to put the extrated date into
				 * @param boundary
				 */
				HttpStatusCode HandleMultipartFormData(String::PeoplezString src, std::vector<PostParam> &dest, String::PeoplezString boundary) noexcept;
				/**
				 * Sorts a header to the right place
				 *
				 * Puts the header with the given information to the right place
				 *
				 * @param name Name of the http header field
				 * @param value Value of the http header field
				 */
				void InsertHeader(String::PeoplezString const name, String::PeoplezString value) noexcept;
				/**
				 * Resets all information to default
				 */
				void Reset();
				virtual ~HttpContext();

				/**
				 * @brief Http request information
				 */
				HttpRequest request;
				/**
				 * @brief Http response information
				 */
				HttpResponse response;
				String::PeoplezString InputBuffer;
				String::PeoplezString OutputBuffer;
				HttpSocketStatus Status;
				bool SendableCBEnabled;
				boost::mutex mut;
				System::IO::Network::Socket * const sender;

			private:
				HttpContext(HttpContext const & other) = delete;

				static const String::PeoplezString COMPARE_STRING_ACCEPT_LANGUAGE;
				static const String::PeoplezString COMPARE_STRING_CONNECTION;
				static const String::PeoplezString COMPARE_STRING_IF_NONE_MATCH;
				static const String::PeoplezString COMPARE_STRING_CONTENT_LENGTH;
				static const String::PeoplezString COMPARE_STRING_COOKIE;
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_HTTPCONTEXT_H_
