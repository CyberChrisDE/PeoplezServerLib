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

//VORSICHT: Klasse ist nur in PeoplezSringBuilder sicher verwendbar!!! (Sonst kann das delete fehlschlagen)

#ifndef PEOPLEZ_STRING_EXTSTRINGELEMENT_H_
#define PEOPLEZ_STRING_EXTSTRINGELEMENT_H_

// Local includes
#include "Element.hpp"

namespace Peoplez
{
	namespace String
	{
		/**
		 * @brief Special element of PeoplezStringBuilder that references to an external text
		 */
		class ExtStringElement final : public Element
		{
		public:
			/**
			 * @brief Initializes the object with a reference to the external text
			 * @details Initializes the object with a relative reference to the external text
			 */
			ExtStringElement(const int position) noexcept : Element(),  pos(position) {}
			virtual ~ExtStringElement() noexcept {}

			void Create(const PeoplezString *src, PeoplezString *tar);
			virtual ExtStringElement* DeepCopy() const noexcept;
			virtual int Length(const PeoplezString *src) const noexcept(noexcept(src->Length()));

		private:
			ExtStringElement(const int position, Element* _next) noexcept : Element(_next), pos(position) {}
			int const pos;
		};
	} // namespace String
} // namespace Peoplez

#endif // PEOPLEZ_STRING_EXTSTRINGELEMENT_H_
