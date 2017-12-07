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

#ifndef PEOPLEZ_SERVICES_HTTP_HTTPREQUEST_H_
#define PEOPLEZ_SERVICES_HTTP_HTTPREQUEST_H_

// Local includes
//#include "../../PeoplezServer/Clients/Client.hpp"
#include "../../String/PeoplezString.hpp"
#include "Enums.hpp"
#include "HttpCookie.hpp"
#include "PostParam.hpp"

// Extern includes
#include <list>
#include <vector>

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			/**
			 * @brief Container for request data
			 */
			class HttpRequest final
			{
				friend class HttpContext;
			public:
				/**
				 * @var typedef boost::unordered_map<String::PeoplezString, String::PeoplezString> StringMap
				 * @brief Map with key and value as PeoplezStrings
				 */
				typedef boost::unordered_map<String::PeoplezString, String::PeoplezString> StringMap;

				/**
				 * Standard constructor
				 */
				HttpRequest() : httpMethod(HttpMethods::UNKNOWN), contentType(MimeType::NONE), keepAlive(true), contentLength(-1), eTag(0) /*isSecureConnection(false), preferredLanguage((Language)-1)*/ {headers.reserve(10);};
				/**
				 * Resets everything to default
				 */
				void Clean();

				/**
				 * Getter for the content length
				 *
				 * @return content length
				 */
				inline uint64_t ContentLength() const noexcept {return contentLength;}
				/**
				 * Determines wheter the content length is set
				 *
				 * Determines wheter a content length > 0 is set
				 */
				inline bool ContentLengthIsSet() const noexcept {return contentLength >= 0;}
				/**
				 * Getter for the request cookies
				 *
				 * Entries are not unescaped
				 *
				 * @return List of all cookies in the request
				 */
				inline std::list<HttpCookie> Cookies() const {return cookies;}
				/**
				 * Getter for the ETag
				 * Default: 0
				 */
				inline size_t ETag() const noexcept {return eTag;}
				/**
				 * Detects the language that fits best
				 *
				 * Detects the language that fits best using the textual representation of the language codes.
				 * If one is found, the numerical representation (given in the map) is returnd, otherwise the default value is returned.
				 * The default value does not need to be included in the map.
				 *
				 * @pre The map must include textual representation of language codes in lower case only
				 *
				 * @param languages Map with language codes
				 * @param def Default result
				 *
				 * @result numerical representation of the best language code
				 */
				int FindBestLanguage(std::map<String::PeoplezString, int> languages, int def);
				/**
				 * Getter for customized headers
				 *
				 * Getter for all header filds that are not handled in an other way
				 *
				 * @return Value of the header if exists; empty string otherwise
				 */
				String::PeoplezString GetHeaderValue(String::PeoplezString const & name);
				/**
				 * Getter for additional request header fields
				 *
				 * Values are both not unescaped, keys aren't
				 *
				 * @return Hash map with the additional request header fields
				 */
				inline std::vector<std::pair<String::PeoplezString, String::PeoplezString> > Headers() const {return headers;}
				/**
				 * Getter for the post parameters from the request
				 *
				 * Keys and values are both NOT unescaped
				 *
				 * @return Hash map with the post parameters from the request
				 */
				inline std::vector<PostParam> PostParams() const {return postParams;}
				/**
				 * Getter for the http method
				 *
				 * @return Type of the request http method
				 */
				inline HttpMethods HttpMethod() const noexcept {return httpMethod;}
		//		/**
		//		 * @brief Getter for security
		//		 * @return true if request went in over a secure connection
		//		 */
		//		inline bool IsSecureConnection(){return isSecureConnection;}
				/**
				 * List of the raw url entries
				 *
				 * List of the raw url entries splitted at each '/'. No empty entries included. Entries are already unescaped.
				 */
				std::vector<String::PeoplezString> rawUrl;
				/**
				 * Destructor
				 *
				 * Cleans up the object by deleting the client
				 */
				virtual ~HttpRequest() noexcept {}

			private:
				HttpMethods httpMethod;
				MimeType contentType;
				bool keepAlive;
				int64_t contentLength;
				String::PeoplezString boundary;
				std::list<HttpCookie> cookies;
				size_t eTag;
				std::vector<std::pair<String::PeoplezString, String::PeoplezString> > headers;
				//bool isSecureConnection;
				std::vector<PostParam> postParams;
				String::PeoplezString userLanguages;
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_HTTPREQUEST_H_
