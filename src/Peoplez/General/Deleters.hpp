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

#ifndef PEOPLEZ_GENERAL_DELETERS_H_
#define PEOPLEZ_GENERAL_DELETERS_H_

namespace Peoplez
{
	namespace General
	{
		/**
		 * @brief Deleter for C Arrays in shared pointers
		 */
		template <typename T>
		class CArrayDeleter final
		{
		public:
			/**
			 * Delete Operator
			 *
			 * @param d Data to be deleted
			 */
			void operator ()(T* d) const noexcept
			{
				free(d);
			}
		};

		/**
		 * @brief Deleter for single indirect pointers
		 */
		template <typename T>
		class PointerToPointerDeleter final
		{
		public:
			/**
			 * Delete Operator
			 *
			 * @param d Data to be deleted
			 */
			void operator ()(T* d) const noexcept
			{
				if(d)
				{
					free(*d);
					free(d);
				}
			}
		};
	} // namespace General
} // namespace Peoplez

#endif // PEOPLEZ_GENERAL_DELETERS_H_
