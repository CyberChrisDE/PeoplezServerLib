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

#ifndef RESOURCEHOLDER_H_
#define RESOURCEHOLDER_H_

// Local includes
#include "../../String/PeoplezString.hpp"
#include "Resource.hpp"

/**
 * @def RESOURCE_TIMEOUT
 * @brief Time in seconds after that a resource should be checked for updates
 */
#define RESOURCE_TIMEOUT 30

namespace Peoplez
{
	namespace Resources
	{
		/**
		 * @brief Container for a resource
		 */
		class ResourceHolder
		{
		public:
			ResourceHolder() : Name(), hash(0), lastModified(0), lastSetup(0) {}
			/**
			 * Recommended constructor
			 *
			 * @param _name Name of the resource for the resource manager
			 */
			ResourceHolder(String::PeoplezString _name) : Name(_name), hash(0), lastModified(0), lastSetup(0) {}
			virtual ~ResourceHolder() {}

			/**
			 * Getter for the resource
			 *
			 * @param hashValue Value of old version. If equal to current, the content will be empty. Default is 0.
			 */
			virtual Resource GetResource(size_t hashValue = 0) = 0;

			/**
			 * @brief Name Name of the resource for the resource handler
			 */
			String::PeoplezString Name;
		protected:
			/**
			 * Checks whether it is needed to check for an update
			 */
			bool NeedsSetup() const {return lastSetup + RESOURCE_TIMEOUT <= time(0);}
			/**
			 * @brief Hash value of the content
			 */
			size_t hash;
			/**
			 * @brief Time at that the content was last modified
			 */
			time_t lastModified;
			/**
			 * @brief Time at that the content was updated the last time
			 */
			time_t lastSetup;
		};
	} // namespace Resources
} // namespace Peoplez

#endif // RESOURCEHOLDER_H_
