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

#ifndef PEOPLEZ_SERVICES_HTTP_HTTPCOOKIE_H_
#define PEOPLEZ_SERVICES_HTTP_HTTPCOOKIE_H_

// Local includes
#include "../../String/PeoplezString.hpp"

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			/**
			 * @brief Represents a single http cookie
			 */
			class HttpCookie
			{
			public:
				/**
				 * Standard constructor
				 *
				 * Initializes the object as an empty cookie
				 */
				HttpCookie() : MaxAge(-1), Secure(false), Discard(false) {}
				/**
				 * Constructor with name and value
				 *
				 * @param name Name of the cookie
				 * @param value Value of the cookie
				 */
				HttpCookie(String::PeoplezString name, String::PeoplezString value) : MaxAge(-1), Name(name), Value(value), Secure(false), Discard(false) {}
				/**
				 * @brief Constructor with name, value and path
				 * @param name Name of the cookie
				 * @param value Value of the cookie
				 * @param path Restricts the validity of the cookie to a specific path
				 * @param maxAge Maximum age until expiration in seconds
				 */
				HttpCookie(String::PeoplezString name, String::PeoplezString value, String::PeoplezString path, int maxAge) : MaxAge(maxAge), Name(name), Value(value), Path(path), Secure(false), Discard(false) {}
				/**
				 * Creates the http header entry for the cookie
				 *
				 * @return Http header entry
				 */
				String::PeoplezString ToString() const;
				virtual ~HttpCookie() {}

				size_t o_ToStringSize() const;

				/**
				 * @brief Comment of the cookie
				 */
				String::PeoplezString Comment;
				//PeoplezString CommentUrl;
				//PeoplezString Domain;
				//bool Expired; //Nicht in rfc2965 definiert
				/**
				 * @brief Maximum age until expiration
				 * @details Maximum age in seconds until the cookie expires. Ignored if negative.
				 */
				int MaxAge;
				/**
				 * @brief Name of the cookie
				 */
				String::PeoplezString Name;
				/**
				 * @brief Value of the cookie
				 */
				String::PeoplezString Value;
				/**
				 * @brief Validity path of the cookie
				 * @details Restricts the validity of the cookie to a specific path (relative paths have to begin with a '/')
				 */
				String::PeoplezString Path;
				/**
				 * @brief Valid ports
				 * @details REstricts the validity of the cookie to the ports
				 */
				String::PeoplezString Port;
				/**
				 * @brief Indicates whether the cookie is only valid on secure connections
				 */
				bool Secure;
				/**
				 * @brief Indicates whether the cookie has to be destroyed when the user agent terminates
				 */
				bool Discard;
				//int Version; //Für erweiterbarkeit drin lassen
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_HTTPCOOKIE_H_
