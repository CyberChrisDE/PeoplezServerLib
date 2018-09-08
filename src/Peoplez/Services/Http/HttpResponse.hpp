/**
 * Copyright 2017, 2018 Christian Geldermann
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

#ifndef PEOPLEZ_SERVICES_HTTP_HTTPRESPONSE_H_
#define PEOPLEZ_SERVICES_HTTP_HTTPRESPONSE_H_

// Local includes
#include "../../String/PeoplezString.hpp"
#include "Enums.hpp"
#include "HttpCookie.hpp"

// Extern includes
#include <list>

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			/**
			 * @enum HttpStatusCode
			 * @brief Status codes for http responses
			 */
			enum class HttpStatusCode : unsigned int
			{
				CONTINUE = 100,
				SWITCHING_PROTOCOL = 101,
				PROCESSING = 102,
				EARLY_HITS = 103,
				OK = 200, //Needs body
				CREATED = 201,
				ACCEPTED = 202,
				NON_AUTH_INFO = 203,
				NO_CONTENT = 204,
				RESET_CONTENT = 205,
				PARTIAL_CONTENT = 206,
				MULTI_STATUS = 207,
				ALREADY_REPORTED = 208,
				IM_USED = 226,
				MULTIPLE_CHOICES = 300,
				MOVED_PERMANENTLY = 301,
				FOUND = 302,
				SEE_OTHER = 303,
				NOT_MODIFIED = 304,
				USE_PROXY = 305,
				SWITCH_PROXY = 306,
				TEMPORARY_REDICT = 307,
				PERMANENT_REDIRECT = 308,
				BAD_REQUEST = 400,
				UNAUTHORIZED = 401,
				PAYMENT_REQUIRED = 402,
				FORBIDDEN = 403,
				NOT_FOUND = 404,
				METHOD_NOT_ALLOWED = 405,
				NOT_ACCEPTABLE = 406,
				PROXY_AUTH_REQUIRED = 407,
				REQUEST_TIMEOUT = 408,
				CONFLICT = 409,
				GONE = 410,
				LENGTH_REQUIRED = 411,
				REQUEST_ENTITY_TOO_LARGE = 413,
				PAYLOAD_TOO_LARGE = 413,
				REQUESTURL_TOO_LONG = 414,
				URI_TOO_LONG = 414,
				UNSUPPORTED_MEDIA_TYPE = 415,
				RANGE_NOT_SATISFIABLE = 416,
				EXPECTATION_FAILED = 417,
				IM_A_TEAPOT = 418,
				THERE_ARE_TOO_MANY_CONNECTIONS_FROM_YOUR_INTERNET_ADDRESS = 421,
				MISDIRECTED_REQUEST = 421,
				UNPROCESSABLE_ENTITY = 422,
				LOCKED = 423,
				FAILED_DEPENDENCY = 424,
				UPGRADE_REQUIRED = 426,
				PRECONDITION_REQUIRED = 428,
				TOO_MANY_REQUESTS = 429,
				REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
				UNAVAILABLE_FOR_LEGAL_REASONS = 451,
				INTERNAL_SERVER_ERROR = 500,
				NOT_IMPLEMENTED = 501,
				BAD_GATEWAY = 502,
				SERVICE_UNAVAILABLE = 503,
				GATEWAY_TIMEOUT = 504,
				HTTP_VERSION_NOT_SUPPORTED = 505,
				VARIANT_ALSO_NEGOTIATES = 506,
				INSUFFICIENT_STORAGE = 507,
				LOOP_DETECTED = 508,
				NOT_EXTENDED = 510,
				NETWORK_AUTH_REQUIRED = 511
			};

			bool isValidStatusCode(HttpStatusCode const statusCode);

			/**
			 * @brief Container for response data
			 * @details Contains all response data and cares of sending the response before being destroyed
			 */
			class HttpResponse final
			{
				friend class HttpContext;
				typedef boost::unordered_map<String::PeoplezString, String::PeoplezString> StringMap;
				typedef std::pair<char const *, size_t> ConstStrLenContainer;
			public:

				HttpResponse();

				/**
				 * Resets everything to default
				 */
				void Clean();
				/**
				 * Generates the complete response text
				 *
				 * Generates the complete text of the response to send to the client/browser
				 *
				 * @return Complete response text
				 */
				String::PeoplezString GetResponseText();
				/**
				 * Sets the status code and the body of the result.
				 *
				 * If the status code is less important than one already set, the whole call is ignored and the old data will stay in whole.
				 * If the data are set and are not overwritten until they are sent, the response will have the given status code and body
				 * and a Content-Lenth header and a Content-Encoding header are automatically added (if needed).
				 * Also a Content-Type header is automatically added with Content-Type: "text/html; charset=UTF-8".
				 * Other headers corresponding to the body or redirect location will not be sent unless they are added manually.
				 *
				 * @param code Status code to send the response with (e.g. OK)
				 * @param body Body of the response
				 * @param bodyLen Size of the body in bytes
				 * @param compr The compression method the body is compressed with
				 */
				void SetWithBody(HttpStatusCode code, char const *body, int bodyLen, HttpCompression compr = HTTP_COMPRESSION_NONE);
				/**
				 * Sets the status code and the body of the result.
				 *
				 * If the status code is less important than one already set, the whole call is ignored and the old data will stay in whole.
				 * If the data are set and are not overwritten until they are sent, the response will have the given status code and body
				 * and a Content-Lenth header and a Content-Encoding header are automatically added (if needed).
				 * Also a Content-Type header is automatically added with Content-Type: "text/html; charset=UTF-8".
				 * Other headers corresponding to the body or redirect location will not be sent unless they are added manually.
				 *
				 * @param code Status code to send the response with (e.g. OK)
				 * @param body Body of the response
				 * @param compr The compression method the body is compressed with
				 */
				void SetWithBody(HttpStatusCode code, String::PeoplezString body, HttpCompression compr = HTTP_COMPRESSION_NONE);
				/**
				 * Sets the status code and the body of the result.
				 *
				 * If the status code is less important than one already set, the whole call is ignored and the old data will stay in whole.
				 * If the data are set and are not overwritten until they are sent, the response will have the given status code and body
				 * and a Content-Lenth header, a Content-Encoding header (if needed) and a Content-Type header are automatically added.
				 * Other headers corresponding to the body or redirect location will not be sent unless they are added manually.
				 *
				 * @param code Status code to send the response with (e.g. OK)
				 * @param contentType Content type of the body
				 * @param body Body of the response
				 * @param compr The compression method the body is compressed with
				 */
				void SetWithBody(HttpStatusCode code, String::PeoplezString contentType, String::PeoplezString body, HttpCompression compr = HTTP_COMPRESSION_NONE);
				/**
				 * Sets the status code and the body of the result.
				 *
				 * If the status code is less important than one already set, the whole call is ignored and the old data will stay in whole.
				 * If the data are set and are not overwritten until they are sent, the response will have the given status code and body
				 * and a Content-Lenth header, a Content-Encoding header (if needed), an ETag header a Content-Type header are automatically added.
				 * Other headers corresponding to the body or redirect location will not be sent unless they are added manually.
				 *
				 * @param code Status code to send the response with (e.g. OK)
				 * @param contentType Content type of the body
				 * @param body Body of the response
				 * @param eTag ETag for caching
				 * @param compr The compression method the body is compressed with
				 */
				void SetWithBody(HttpStatusCode code, String::PeoplezString contentType, String::PeoplezString body, size_t eTag, HttpCompression compr);
				/**
				 * Sets a redict as answer
				 *
				 * @param code Status code of the response (should be a code for redicting)
				 * @param location Absolute or relative location of the target
				 */
				void SetRedict(HttpStatusCode code, String::PeoplezString location);
				/**
				 * Sets a status code not needing a body
				 *
				 * Intended for error codes
				 *
				 * @param code Status code of the response (should be an error code)
				 */
				void SetOther(HttpStatusCode code);
				virtual ~HttpResponse() {}

				/**
				 * @brief List of response cookies
				 */
				std::list<HttpCookie> Cookies;
				/**
				 * @brief List of additional response header fields
				 */
				StringMap Headers;
				/**
				 * @brief Indicates whether a KeepAlive header is sent with the response
				 */
				bool KeepAlive;

			private:
				//String::PeoplezString CreateCompleteHeader();
				/**
				 * Creates the header entries in the "Headers" field
				 *
				 * @return Generated header entries for the response
				 */
				String::PeoplezString CreateHeaders() const;
				/**
				 * Creates the cookie header fields
				 *
				 * @return Header entries for the cookies
				 */
				String::PeoplezString CreateCookies() const;
				size_t GetCookiesSize() const;
				size_t GetHeadersSize() const;
				/**
				 * Detects the status description of the set status code
				 *
				 * @return Description of the set status code
				 */
				ConstStrLenContainer GetStatusDescription() const;
				/**
				 * Detects whether the set response code is a redict code
				 *
				 * @return true if the set response code is a redict code, false otherwise
				 */
				bool IsRedict() noexcept __attribute__((pure));
				/**
				 * Resets the body, contentType and location to their default values
				 */
				void ResetBodyAndLocation() noexcept;
				/**
				 * Sets the given status code if it is the most important one
				 *
				 * Checks if the given status code has a higher priority than the old one and sets it if so.
				 *
				 * @return true if new status code is set, false otherwise (true if old and new status codes are the same ones)
				 */
				bool SetStatusCode(HttpStatusCode code);

				HttpCompression compression;
				String::PeoplezString contentType;
				String::PeoplezString data;
				/**
				 * @brief Indicates whether data are already set
				 */
				bool dataSet;
				size_t eTag;
				String::PeoplezString redirectLocation;
				HttpStatusCode statusCode;
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_HTTPRESPONSE_H_
