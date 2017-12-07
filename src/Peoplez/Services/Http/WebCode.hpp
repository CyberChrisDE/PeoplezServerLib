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

#ifndef PEOPLEZ_SERVICES_HTTP_WEBCODE_H_
#define PEOPLEZ_SERVICES_HTTP_WEBCODE_H_

// Local includes
#include "../../String/PeoplezString.hpp"

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			/**
			 * @brief Container for all web code
			 * @details Container including all web code for a specific page or page part parted to html, css and script code.
			 */
			class WebCode final
			{
			public:
				/**
				 * Standard constructor
				 *
				 * Initializes the object with empty strings
				 */
				WebCode() {}
				/**
				 * Constructor for script less code
				 *
				 * Initializes the object with the given html and css code and an empty string for script
				 *
				 * @param htmlCode Html part of the code
				 * @param cssCode Css part of the code
				 */
				WebCode(String::PeoplezString htmlCode, String::PeoplezString cssCode) : html(htmlCode), css(cssCode) {}
				/**
				 * Constructor for code including html, css and script
				 *
				 * Initializes the object with the given html, css and script code
				 *
				 * @param htmlCode Html part of the code
				 * @param cssCode Css part of the code
				 * @param scriptCode Script part of the code
				 */
				WebCode(String::PeoplezString htmlCode, String::PeoplezString cssCode, String::PeoplezString scriptCode) : html(htmlCode), css(cssCode), script(scriptCode) {}
				virtual ~WebCode() {}

				/**
				 * Appends code to the object
				 *
				 * Appends the given code to the object by appending the strings (html, css and script) their according strings each.
				 *
				 * @param source Code to be appended
				 */
				void operator+=(const WebCode &source);

				/**
				 * @brief html part of the web code
				 */
				String::PeoplezString html;
				/**
				 * @brief css part of the web code
				 */
				String::PeoplezString css;
				/**
				 * @brief script part of the web code
				 */
				String::PeoplezString script;
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_WEBCODE_H_
