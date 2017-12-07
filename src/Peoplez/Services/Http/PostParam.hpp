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

#ifndef PEOPLEZ_SERVICES_HTTP_POSTPARAM_H_
#define PEOPLEZ_SERVICES_HTTP_POSTPARAM_H_

// Local includes
#include "../../String/PeoplezString.hpp"
#include "../../General/Enums.hpp"

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			/**
			 * @brief Post parameter of an http message
			 */
			class PostParam final
			{
			public:
				PostParam() : Mime(MimeType::NONE) {};
				/**
				 * Constructor
				 *
				 * @param name
				 * @param value
				 * @param mime
				 */
				PostParam(String::PeoplezString name, String::PeoplezString value, MimeType const mime = MimeType::NONE) : Name(name), Value(value), Mime(mime) {}
				/**
				 * Constructor
				 *
				 * @param name
				 * @param value
				 * @param fileName
				 * @param mime
				 */
				PostParam(String::PeoplezString name, String::PeoplezString value, String::PeoplezString fileName, MimeType const mime = MimeType::NONE) : Name(name), Value(value), FileName(fileName), Mime(mime) {}
				virtual ~PostParam() {};

				/**
				 * @brief Name of the post parameter
				 */
				String::PeoplezString Name;
				/**
				 * @brief Value of the post parameter
				 */
				String::PeoplezString Value;
				String::PeoplezString FileName;
				MimeType Mime;
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_POSTPARAM_H_
