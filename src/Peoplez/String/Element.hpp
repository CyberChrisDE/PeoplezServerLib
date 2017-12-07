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

#ifndef ELEMENT_H_
#define ELEMENT_H_

// Local includes
#include "PeoplezString.hpp"

namespace Peoplez
{
	namespace String
	{
		/**
		 * @brief Abstract class for elements in PeoplezStringBuilder
		 */
		class Element
		{
		public:
			/**
			 * @brief Appends the given element at the end of the list
			 * @details Calls the Append function of the child recursively if it has one. Otherwise it makes the given element to its child.
			 * @param element Element to be added at the end of the list
			 */
			void Append(Element* element) noexcept
			{
				if(!next) next = element;
				else next->Append(element);
			}
			/**
			 * @brief Creates the resulting string
			 * @details Inserts its text at the end of the target and calls its childs Create function recursively
			 * @param src Array of values the text is created of
			 * @param tar Pointer to the target string the text is written to
			 */
			virtual void Create(const PeoplezString *src, PeoplezString *tar) = 0;
			/**
			 * @brief Cleans up the element
			 */
			void Dispose() noexcept(std::is_nothrow_destructible<Element>::value)
			{
				delete next;
				next = 0;
			}
			/**
			 * @brief Creates a deep copy of the list beginning at this object
			 * @return Deep copied list
			 */
			virtual Element* DeepCopy() noexcept(std::is_nothrow_copy_constructible<PeoplezString>::value) = 0;
			/**
			 * @brief Calculates the length of the target text
			 * @details Calculates the length of the target text by recursively calling the Length functions of the childs and adding their result to the own text length.
			 * @param src Array of values the text is created of
			 * @return Length of the resulting target text
			 */
			virtual int Length(const PeoplezString *src) noexcept(noexcept(src->Length())) = 0;
			virtual ~Element() noexcept(noexcept(std::is_nothrow_destructible<PeoplezString>::value)) {Dispose();}

		protected:
			/**
			 * Constructor to use from Childs
			 *
			 * @param _next Pointer to next element in linked list
			 */
			Element(Element * _next = 0) noexcept : next(_next){};
			/**
			 * @brief Pointer to the next element of the list
			 */
			Element *next;
		};
	} // namespace String
} // namespace Peoplez

#endif // ELEMENT_H_
