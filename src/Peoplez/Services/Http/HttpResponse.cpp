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

// Own includes
#include "HttpResponse.hpp"

// Local includes
#include "../../String/Parsing/IntToString.hpp"
#include "../../System/Logging/Logger.hpp"

// Extern includes
#include <iostream>
#include <sstream>
#include <cstdlib>
extern "C"
{
#include <sys/socket.h>
#include <unistd.h>
}

namespace Peoplez
{
	// Local namespaces
	using namespace String;
	using namespace String::Parsing;
	using namespace System::Logging;

	// External namespaces
	using namespace std;

	namespace Services
	{
		namespace Http
		{
			static char const * const HTTP_RESPONSE_CONTENT_TYPE_DEFAULT = "text/html; charset=UTF-8";
			static size_t const HTTP_RESPONSE_CONTENT_TYPE_DEFAULT_LEN = 24;

			bool isValidStatusCode(HttpStatusCode const statusCode)
			{
				switch(statusCode)
				{
				case HttpStatusCode::CONTINUE:
				case HttpStatusCode::SWITCHING_PROTOCOL:
				case HttpStatusCode::PROCESSING:
				case HttpStatusCode::EARLY_HITS:
				case HttpStatusCode::OK:
				case HttpStatusCode::CREATED:
				case HttpStatusCode::ACCEPTED:
				case HttpStatusCode::NON_AUTH_INFO:
				case HttpStatusCode::NO_CONTENT:
				case HttpStatusCode::RESET_CONTENT:
				case HttpStatusCode::PARTIAL_CONTENT:
				case HttpStatusCode::MULTI_STATUS:
				case HttpStatusCode::ALREADY_REPORTED:
				case HttpStatusCode::IM_USED:
				case HttpStatusCode::MULTIPLE_CHOICES:
				case HttpStatusCode::MOVED_PERMANENTLY:
				case HttpStatusCode::FOUND:
				case HttpStatusCode::SEE_OTHER:
				case HttpStatusCode::NOT_MODIFIED:
				case HttpStatusCode::USE_PROXY:
				case HttpStatusCode::SWITCH_PROXY:
				case HttpStatusCode::TEMPORARY_REDICT:
				case HttpStatusCode::PERMANENT_REDIRECT:
				case HttpStatusCode::BAD_REQUEST:
				case HttpStatusCode::UNAUTHORIZED:
				case HttpStatusCode::PAYMENT_REQUIRED:
				case HttpStatusCode::FORBIDDEN:
				case HttpStatusCode::NOT_FOUND:
				case HttpStatusCode::METHOD_NOT_ALLOWED:
				case HttpStatusCode::NOT_ACCEPTABLE:
				case HttpStatusCode::PROXY_AUTH_REQUIRED:
				case HttpStatusCode::REQUEST_TIMEOUT:
				case HttpStatusCode::CONFLICT:
				case HttpStatusCode::GONE:
				case HttpStatusCode::LENGTH_REQUIRED:
				case HttpStatusCode::PAYLOAD_TOO_LARGE:
				case HttpStatusCode::URI_TOO_LONG:
				case HttpStatusCode::UNSUPPORTED_MEDIA_TYPE:
				case HttpStatusCode::RANGE_NOT_SATISFIABLE:
				case HttpStatusCode::EXPECTATION_FAILED:
				case HttpStatusCode::IM_A_TEAPOT:
				case HttpStatusCode::MISDIRECTED_REQUEST:
				case HttpStatusCode::UNPROCESSABLE_ENTITY:
				case HttpStatusCode::LOCKED:
				case HttpStatusCode::FAILED_DEPENDENCY:
				case HttpStatusCode::UPGRADE_REQUIRED:
				case HttpStatusCode::PRECONDITION_REQUIRED:
				case HttpStatusCode::TOO_MANY_REQUESTS:
				case HttpStatusCode::REQUEST_HEADER_FIELDS_TOO_LARGE:
				case HttpStatusCode::UNAVAILABLE_FOR_LEGAL_REASONS:
				case HttpStatusCode::INTERNAL_SERVER_ERROR:
				case HttpStatusCode::NOT_IMPLEMENTED:
				case HttpStatusCode::BAD_GATEWAY:
				case HttpStatusCode::SERVICE_UNAVAILABLE:
				case HttpStatusCode::GATEWAY_TIMEOUT:
				case HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED:
				case HttpStatusCode::VARIANT_ALSO_NEGOTIATES:
				case HttpStatusCode::INSUFFICIENT_STORAGE:
				case HttpStatusCode::LOOP_DETECTED:
				case HttpStatusCode::NOT_EXTENDED:
				case HttpStatusCode::NETWORK_AUTH_REQUIRED:
					return true;
				default:
					return false;
				}
			}

			HttpResponse::HttpResponse() : KeepAlive(true), compression(HTTP_COMPRESSION_NONE), contentType(HTTP_RESPONSE_CONTENT_TYPE_DEFAULT, HTTP_RESPONSE_CONTENT_TYPE_DEFAULT_LEN), dataSet(false), eTag(0), statusCode(HttpStatusCode::OK)
			{
			}

			void HttpResponse::Clean()
			{
				Cookies.clear();
				Headers.clear();
				data.Clear();
				contentType.SetTo(HTTP_RESPONSE_CONTENT_TYPE_DEFAULT, HTTP_RESPONSE_CONTENT_TYPE_DEFAULT_LEN);
				redirectLocation.Clear();
				statusCode = HttpStatusCode::OK;
				dataSet = false;
			}

			size_t HttpResponse::GetCookiesSize() const
			{
				size_t res = 0;

				for(std::list<HttpCookie>::const_iterator iter = Cookies.begin(); iter != Cookies.end(); ++iter)
				{
					res += iter->o_ToStringSize() + 2;
				}

				return res;
			}

			size_t HttpResponse::GetHeadersSize() const
			{
				if(Headers.empty()) return 0;

				size_t res = 0;
				unordered_map<PeoplezString, PeoplezString>::const_iterator iter = Headers.begin();

				while(iter != Headers.end()) res += 3 + iter->first.Length() + iter->second.Length();

				return res;
			}

			PeoplezString HttpResponse::GetResponseText()
			{
				bool const isRedict = IsRedict();
				char _binaryDataLength[21];
				int dataLengthSize = 0;
				//Größe berechnen
				size_t size = 0;

				char _statusCode[5];
				const int statusCodeSize = 1 + ToCStringBase10(_statusCode, (unsigned int)statusCode);
				_statusCode[statusCodeSize - 1] = ' ';
				size += statusCodeSize;

				//if(isRedict) size += 12 + redirectLocation.Length();
				//else if(eTag != 0) size += 10 + (sizeof(eTag) << 1);
				size += isRedict ? (12 + redirectLocation.Length()) : (eTag != 0 ? (10 + (sizeof(eTag) << 1)) : 0);

				if(statusCode != HttpStatusCode::NOT_MODIFIED)
				{
					size += 34 + contentType.Length();

					dataLengthSize = ToCStringBase10(_binaryDataLength, data.Length());
					size += dataLengthSize;

					if(compression == HTTP_COMPRESSION_DEFLATE) size += 27;
					else if(compression == HTTP_COMPRESSION_GZIP) size += 24;
				}

				//if(KeepAlive) size += 28;
				size += KeepAlive ? 28 : 0;

				ConstStrLenContainer status = GetStatusDescription();
				size += status.second;

				size_t const cookiesSize = GetCookiesSize();
				size_t const headersSize = GetHeadersSize();
				size += cookiesSize + headersSize;

				size += data.Length();

				//Teile zusammenfügen

				PeoplezString result(size + 32);

				result.Append("HTTP/1.1 ", 9); // 9
				result.Append(_statusCode, statusCodeSize); //statusCodeSize
				if(status.second) result.Append(status.first, status.second); //status.Length()
				if(isRedict)
				{
					result.Append("\r\nLocation: ", 12); // 12
					result.Append(redirectLocation); // redirectLocation.Length()
				}
				else if(eTag != 0)
				{
					result.Append("\r\nEtag: \"", 9); // 9
					result.Append(PeoplezString::ParseHex(eTag)); // sizeof(eTag) << 1
					result.Append("\"", 1); // 1
				}

				if(statusCode != HttpStatusCode::NOT_MODIFIED)
				{
					result.Append("\r\nContent-Type: ", 16); // 16
					result.Append(contentType); // contentType.Length()
					if(compression == HTTP_COMPRESSION_GZIP) result.Append("\r\nContent-Encoding: gzip", 24); // 24
					else if(compression == HTTP_COMPRESSION_DEFLATE) result.Append("\r\nContent-Encoding: deflate", 27); // 27
					result.Append("\r\nContent-Length: ", 18); // 18
					result.Append(_binaryDataLength, dataLengthSize); // dataLengthSize
				}
				result.Append("\r\nConnection: ", 14); // 14
				if(KeepAlive) result.Append("Keep-Alive\r\nKeep-Alive: timeout=5\r\n", 35); // 35
				else result.Append("close\r\n", 7); // 7
				if(cookiesSize > 0) result.Append(CreateCookies()); // _cookies.Length()
				if(headersSize > 0) result.Append(CreateHeaders()); // _headers.Length()
				result.Append("\r\n", 2); // 2
				result.Append(data); // data.Length()

				return result;
			}

			void HttpResponse::SetWithBody(HttpStatusCode const code, char const * const body, int const bodyLen, HttpCompression const compr)
			{
				SetWithBody(code, PeoplezString(HTTP_RESPONSE_CONTENT_TYPE_DEFAULT, HTTP_RESPONSE_CONTENT_TYPE_DEFAULT_LEN), PeoplezString(body, bodyLen), compr);
			}

			void HttpResponse::SetWithBody(HttpStatusCode const code, PeoplezString const body, HttpCompression const compr)
			{
				SetWithBody(code, PeoplezString(HTTP_RESPONSE_CONTENT_TYPE_DEFAULT, HTTP_RESPONSE_CONTENT_TYPE_DEFAULT_LEN), body, compr);
			}

			void HttpResponse::SetWithBody(HttpStatusCode const code, PeoplezString const _contentType, PeoplezString const body, HttpCompression const compr)
			{
				if(SetStatusCode(code))
				{
					ResetBodyAndLocation();

					compression = compr;
					contentType = _contentType;
					data = body;
					eTag = 0;
				}
			}

			void HttpResponse::SetWithBody(HttpStatusCode const code, PeoplezString const _contentType, PeoplezString const body, size_t const _eTag, HttpCompression const compr)
			{
				if(SetStatusCode(code))
				{
					ResetBodyAndLocation();

					compression = compr;
					contentType = _contentType;
					data = body;
					eTag = _eTag;
				}
			}

			void HttpResponse::SetRedict(HttpStatusCode const code, PeoplezString const location)
			{
				if(SetStatusCode(code))
				{
					ResetBodyAndLocation();
					redirectLocation = location;
				}
			}

			void HttpResponse::SetOther(HttpStatusCode const code)
			{
				ResetBodyAndLocation();
				SetStatusCode(code);
			}

			PeoplezString HttpResponse::CreateHeaders() const
			{
				PeoplezString result;
				unordered_map<PeoplezString, PeoplezString>::const_iterator iter = Headers.begin();

				while(iter != Headers.end()) result.Append(iter->first + ":" + iter->second + "\r\n");

				return result;
			}

			PeoplezString HttpResponse::CreateCookies() const
			{
				PeoplezString result;

				for(std::list<HttpCookie>::const_iterator iter = Cookies.begin(); iter != Cookies.end(); ++iter)
				{
					result.Append(iter->ToString());
					result.Append("\r\n");
				}

				return result;
			}

			HttpResponse::ConstStrLenContainer HttpResponse::GetStatusDescription() const
			{
				switch(statusCode)
				{
				case HttpStatusCode::CONTINUE:
					return ConstStrLenContainer("Continue", 8);
				case HttpStatusCode::SWITCHING_PROTOCOL:
					return ConstStrLenContainer("Switching Protocols", 19);
				case HttpStatusCode::PROCESSING:
					return ConstStrLenContainer("Processing", 10);
				case HttpStatusCode::EARLY_HITS:
					return ConstStrLenContainer("Early Hits", 10);
				case HttpStatusCode::OK:
					return ConstStrLenContainer("OK", 2);
				case HttpStatusCode::CREATED:
					return ConstStrLenContainer("Created", 7);
				case HttpStatusCode::ACCEPTED:
					return ConstStrLenContainer("Accepted", 8);
				case HttpStatusCode::NON_AUTH_INFO:
					return ConstStrLenContainer("Non-Authoritative Information", 29);
				case HttpStatusCode::NO_CONTENT:
					return ConstStrLenContainer("No Content", 10);
				case HttpStatusCode::RESET_CONTENT:
					return ConstStrLenContainer("Reset Content", 13);
				case HttpStatusCode::PARTIAL_CONTENT:
					return ConstStrLenContainer("Partial Content", 15);
				case HttpStatusCode::MULTI_STATUS:
					return ConstStrLenContainer("Multi-Status", 12);
				case HttpStatusCode::ALREADY_REPORTED:
					return ConstStrLenContainer("Already Reported", 16);
				case HttpStatusCode::IM_USED:
					return ConstStrLenContainer("IM Used", 7);
				case HttpStatusCode::MULTIPLE_CHOICES:
					return ConstStrLenContainer("Multiple Choices", 16);
				case HttpStatusCode::MOVED_PERMANENTLY:
					return ConstStrLenContainer("Moved Permanently", 17);
				case HttpStatusCode::FOUND:
					return ConstStrLenContainer("Found", 5);
				case HttpStatusCode::SEE_OTHER:
					return ConstStrLenContainer("See Other", 9);
				case HttpStatusCode::NOT_MODIFIED:
					return ConstStrLenContainer("Not Modified", 12);
				case HttpStatusCode::USE_PROXY:
					return ConstStrLenContainer("Use Proxy", 9);
				case HttpStatusCode::SWITCH_PROXY:
					return ConstStrLenContainer("Switch Proxy", 12);
				case HttpStatusCode::TEMPORARY_REDICT:
					return ConstStrLenContainer("Temporary Redict", 16);
				case HttpStatusCode::PERMANENT_REDIRECT:
					return ConstStrLenContainer("Permanent Redirect", 18);
				case HttpStatusCode::BAD_REQUEST:
					return ConstStrLenContainer("Bad Request", 11);
				case HttpStatusCode::UNAUTHORIZED:
					return ConstStrLenContainer("Unauthorized", 12);
				case HttpStatusCode::PAYMENT_REQUIRED:
					return ConstStrLenContainer("Payment Required", 16);
				case HttpStatusCode::FORBIDDEN:
					return ConstStrLenContainer("Forbidden", 9);
				case HttpStatusCode::NOT_FOUND:
					return ConstStrLenContainer("Not Found", 9);
				case HttpStatusCode::METHOD_NOT_ALLOWED:
					return ConstStrLenContainer("Method Not Allowed", 18);
				case HttpStatusCode::NOT_ACCEPTABLE:
					return ConstStrLenContainer("Not Acceptable", 14);
				case HttpStatusCode::PROXY_AUTH_REQUIRED:
					return ConstStrLenContainer("Proxy Authentication Required", 29);
				case HttpStatusCode::REQUEST_TIMEOUT:
					return ConstStrLenContainer("Request Time-out", 16);
				case HttpStatusCode::CONFLICT:
					return ConstStrLenContainer("Conflict", 8);
				case HttpStatusCode::GONE:
					return ConstStrLenContainer("Gone", 4);
				case HttpStatusCode::LENGTH_REQUIRED:
					return ConstStrLenContainer("Length Required", 15);
				case HttpStatusCode::PAYLOAD_TOO_LARGE:
					return ConstStrLenContainer("Payload Too Large", 17);
				case HttpStatusCode::URI_TOO_LONG:
					return ConstStrLenContainer("URI Too Long", 12);
				case HttpStatusCode::UNSUPPORTED_MEDIA_TYPE:
					return ConstStrLenContainer("Unsupported Media Type", 22);
				case HttpStatusCode::RANGE_NOT_SATISFIABLE:
					return ConstStrLenContainer("Range Not Satisfiable", 21);
				case HttpStatusCode::EXPECTATION_FAILED:
					return ConstStrLenContainer("Expectation Failed", 18);
				case HttpStatusCode::IM_A_TEAPOT:
					return ConstStrLenContainer("I'm a teapot", 12);
				case HttpStatusCode::MISDIRECTED_REQUEST:
					return ConstStrLenContainer("Misdirected Request", 19);
				case HttpStatusCode::UNPROCESSABLE_ENTITY:
					return ConstStrLenContainer("Unsupported Media Type", 22);
				case HttpStatusCode::LOCKED:
					return ConstStrLenContainer("Locked", 6);
				case HttpStatusCode::FAILED_DEPENDENCY:
					return ConstStrLenContainer("Failed Dependency", 17);
				case HttpStatusCode::UPGRADE_REQUIRED:
					return ConstStrLenContainer("Upgrade Required", 16);
				case HttpStatusCode::PRECONDITION_REQUIRED:
					return ConstStrLenContainer("Precondition Required", 21);
				case HttpStatusCode::TOO_MANY_REQUESTS:
					return ConstStrLenContainer("Too Many Requests", 17);
				case HttpStatusCode::REQUEST_HEADER_FIELDS_TOO_LARGE:
					return ConstStrLenContainer("Request Header Fields Too Large", 31);
				case HttpStatusCode::UNAVAILABLE_FOR_LEGAL_REASONS:
					return ConstStrLenContainer("Unavailable For Legal Reasons", 29);
				case HttpStatusCode::INTERNAL_SERVER_ERROR:
					return ConstStrLenContainer("Internal Server Error", 21);
				case HttpStatusCode::NOT_IMPLEMENTED:
					return ConstStrLenContainer("Not Implemented", 15);
				case HttpStatusCode::BAD_GATEWAY:
					return ConstStrLenContainer("Bad Gateway", 11);
				case HttpStatusCode::SERVICE_UNAVAILABLE:
					return ConstStrLenContainer("Service Unavailable", 19);
				case HttpStatusCode::GATEWAY_TIMEOUT:
					return ConstStrLenContainer("Gateway Timeout", 15);
				case HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED:
					return ConstStrLenContainer("HTTP Version not supported", 26);
				case HttpStatusCode::VARIANT_ALSO_NEGOTIATES:
					return ConstStrLenContainer("Variant Also Negotiates", 23);
				case HttpStatusCode::INSUFFICIENT_STORAGE:
					return ConstStrLenContainer("Insufficient Storage", 20);
				case HttpStatusCode::LOOP_DETECTED:
					return ConstStrLenContainer("Loop Detected", 13);
				case HttpStatusCode::NOT_EXTENDED:
					return ConstStrLenContainer("Not Extended", 12);
				case HttpStatusCode::NETWORK_AUTH_REQUIRED:
					return ConstStrLenContainer("Network Authentication Required", 31);
				default:
					Logger::LogException("No description for this status code available", __FILE__, __LINE__);
					return ConstStrLenContainer(0, 0);
				}
			}

			bool HttpResponse::IsRedict() noexcept
			{
				switch(statusCode)
				{
				case HttpStatusCode::MOVED_PERMANENTLY:
				case HttpStatusCode::SEE_OTHER:
				case HttpStatusCode::TEMPORARY_REDICT:
					return true;
				default:
					return false;
				}
			}

			void HttpResponse::ResetBodyAndLocation() noexcept
			{
				redirectLocation.Clear();
				contentType.SetTo(HTTP_RESPONSE_CONTENT_TYPE_DEFAULT, HTTP_RESPONSE_CONTENT_TYPE_DEFAULT_LEN);
				eTag = 0;
			}

			bool HttpResponse::SetStatusCode(HttpStatusCode const code)
			{
				if(!dataSet)
				{
					statusCode = code;
					dataSet = true;
					return true;
				}
				else if(((unsigned int)code >= 400 && (unsigned int)statusCode < 400))
				{
					Logger::LogEvent("Status-Code already set. New code set.");
					statusCode = code;
					return true;
				}
				else
				{
					Logger::LogEvent("Status-Code already set. New code not set.");
					return false;
				}
			}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
