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

#ifndef PEOPLEZ_SERVICES_HTTP_HTTPFUNCTIONS_H_
#define PEOPLEZ_SERVICES_HTTP_HTTPFUNCTIONS_H_

// Local includes
#include "../../String/PeoplezString.hpp"
#include "../../General/Enums.hpp"
#include "Enums.hpp"
#include "FileType.hpp"

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			/**
			 * @brief Static Helper class for http functionalities
			 */
			class HttpFunctions final
			{
			public:
				/**
				 * Identifies the language code to enum Language entry
				 *
				 * Reads the first language code in the given array and returns the language that is represented by it
				 *
				 * @param lang Char array to transform
				 *
				 * @return Language that is represented in the given array
				 */
				static Language IdentifyLanguage(char const *lang) noexcept __attribute__((pure));
				/**
				 * Reads in a binary file
				 *
				 * @param path Path to the file
				 * @param data Pointer to the location where to create the result
				 *
				 * @return Number of bytes read
				 */
				static int ReadBinaryFile(String::PeoplezString path, char **data);
				/**
				 * Reads a string from a file and shortens it
				 *
				 * @param path Path to the file
				 * @param beginsWith Sort of code the file begins with
				 *
				 * @return Shortened text from file
				 */
				static String::PeoplezString ReadFile(String::PeoplezString path, FileBeginsWith beginsWith);
				static String::PeoplezString FileTypeToContentType(FileType type);
				/**
				 * Identifies the http method represented in the given string
				 *
				 * @param str String containg the textual representation of the http method
				 *
				 * @return HttpMethod that is represented in the given string
				 */
				static HttpMethods ToHttpMethod(String::PeoplezString const & str) __attribute__((pure));

				static String::PeoplezString ToPString(HttpMethods const method) __attribute__((pure));

			private:
		 		/**
		 		 * Shortens the given text
		 		 *
		 		 * @param beginsWith Type of the sort of text it begins with (HTML, CSS, SCRIPT)
		 		 *
		 		 * @rerun Minimized text
		 		 */
				static String::PeoplezString Shorten(String::PeoplezString text, Services::Http::FileBeginsWith beginsWith);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning with html code
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenHtml(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning in an html tag
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenHtmlTag(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning with css code
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenCss(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning in a css area
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenCssArea(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning with a css tag
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenCssTag(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning with plain text
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenText(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning with script code
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenScript(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning in a script function
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenScriptFunction(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning with a script tag
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenScriptTag(char ** text, char * end);
				static int ShortenHtmlComment(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning in a multiline comment
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenCommentMultiLine(char ** text, char * end);
		 		/**
		 		 * Help function of Shorten
		 		 *
		 		 * Shortens text beginning with a single line comment
		 		 *
		 		 * @return Number of erased characters
		 		 */
				static int ShortenCommentSingleLine(char ** text, char * end);
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_HTTPFUNCTIONS_H_
