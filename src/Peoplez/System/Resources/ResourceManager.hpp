/**
 * Copyright 2017, 2019 Christian Geldermann
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

#ifndef PEOPLEZ_SYSTEM_RESOURCES_RESSOURCEMANAGER_H_
#define PEOPLEZ_SYSTEM_RESOURCES_RESSOURCEMANAGER_H_

// Local includes
#include "../../Services/Http/HttpContext.hpp"
#include "ResourceHolder.hpp"

// Extern includes
#include <mutex>

namespace Peoplez
{
	namespace System
	{
		namespace Resources
		{
			/**
			 * @brief Manages all resources the requests to them
			 */
			class ResourceManager final
			{
			public:
				/**
				 * Constructor
				 *
				 * @param resourceDirectory Path to resources. Has to end with a '/'.
				 */
				ResourceManager(String::PeoplezString const resourceDirectory)
					: directory(resourceDirectory) {Initialize();}
				/**
				 * Initializer
				 */
				void Initialize() noexcept;
				/**
				 * Determins the content of a file in the public folder
				 *
				 * @pre fileName must be in text mode and the right to open such a file type must exist
				 *
				 * @param fileName Name of the file (including file extension)
				 * @param hash Hash value of the resource at the client side (0 if none)
				 *
				 * @return Content of the file (empty string when failed)
				 */
				Resource GetResource(String::PeoplezString fileName, size_t hash);
				//FileSaveStatus SaveFile(uint64_t userID, String::PeoplezString fileName, String::PeoplezString content) noexcept;
				~ResourceManager() noexcept;
			private:
				String::PeoplezString directory;
				std::vector<ResourceHolder *> resources;
				std::mutex resourceMutex;
			};
		} // namespace Resources
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_RESOURCES_RESSOURCEMANAGER_H_
