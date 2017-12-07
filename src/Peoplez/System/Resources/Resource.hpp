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

#ifndef RESOURCE_H_
#define RESOURCE_H_

// Local includes
#include "../../String/PeoplezString.hpp"
#include "../../General/Enums.hpp"
#include "Enums.hpp"

namespace Peoplez
{
	namespace Resources
	{
		/**
		 * @brief Return type of resource holders
		 */
		class Resource final
		{
		public:
			Resource() : Compressed(false), Hash(0), Status(RESOURCE_STATUS_NOT_MODIFIED), Type(FILE_TYPE::NONE) {}
			/**
			 * Recommended constructor
			 *
			 * @param compressed Indicates whether the content is compressed
			 * @param content Content itself
			 * @param status Modification status of the content
			 * @param hash Hash value of the content
			 * @param type Type of the content
			 */
			Resource(bool compressed, String::PeoplezString content, ResourceStatus status, size_t hash, FileType type) : Compressed(compressed), Content(content), Hash(hash), Status(status), Type(type) {}
			virtual ~Resource() {}

			/**
			 * @brief Indicates whether the content is compressed
			 */
			bool Compressed;
			/**
			 * @brief Content itself
			 */
			String::PeoplezString Content;
			/**
			 * @brief Hash value of the content
			 */
			size_t Hash;
			/**
			 * @brief Modification status of the content
			 */
			ResourceStatus Status;
			/**
			 * @brief Type of the content
			 */
			FileType Type;
		};
	} // namespace Resources
} // namespace Peoplez

#endif // RESOURCE_H_
