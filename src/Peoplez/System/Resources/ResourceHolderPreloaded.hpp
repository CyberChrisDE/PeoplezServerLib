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

#ifndef PEOPLEZ_SYSTEM_RESOURCES_RESOURCEHOLDERPRELOADED_H_
#define PEOPLEZ_SYSTEM_RESOURCES_RESOURCEHOLDERPRELOADED_H_

// Local includes
#include "ResourceHolder.hpp"

// Extern includes
#include <boost/thread/mutex.hpp>

namespace Peoplez
{
	namespace System
	{
		namespace Resources
		{
			/**
			 * @brief Resource holder that caches the content in memory
			 */
			class ResourceHolderPreloaded final : public ResourceHolder
			{
			public:
				/**
				 * Constructor
				 *
				 * @param directory The relative path to the directory in that the file is
				 * @param fileName Name of the file containing the content
				 */
				ResourceHolderPreloaded(String::PeoplezString directory, String::PeoplezString fileName);
				virtual Resource GetResource(size_t hashValue) noexcept(noexcept(Resource(false, "", RESOURCE_STATUS_ERROR, 0, FILE_TYPE::NONE)));
				virtual ~ResourceHolderPreloaded() {}

			protected:
				void UpdateHashValue() {}
				/**
				 * @brief Indicates whether the content is compressed
				 */
				bool compressed;
				/**
				 * @brief The content itself
				 */
				String::PeoplezString content;
				/**
				 * @brief General mutex
				 */
				boost::mutex contentMutex;
				/**
				 * @brief Relative path of the file containing the resource content
				 */
				String::PeoplezString Path;
				/**
				 * @brief Type of the content
				 */
				FileType type;
			};
		} // namespace Resources
	}
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_RESOURCES_RESOURCEHOLDERPRELOADED_H_
