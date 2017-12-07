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

#ifndef PEOPLEZ_STRING_PEOPLEZSTRINGBUILDERDATA_H_
#define PEOPLEZ_STRING_PEOPLEZSTRINGBUILDERDATA_H_

// Local includes
#include "Element.hpp"

// Extern includes
#include <boost/shared_array.hpp>
#include <memory>

namespace Peoplez
{
	namespace String
	{
		/**
		 * @brief Collects the new text parts for PeoplezStringBuilders
		 */
		class PeoplezStringBuilderData final
		{
			friend class PeoplezStringBuilder;
			friend class ConstStringBuilder;
		public:
			/**
			 * Copy constructor
			 *
			 * @param original Instance that should get copied
			 */
			PeoplezStringBuilderData(PeoplezStringBuilderData const & original);
			/**
			 * Destructor
			 */
			virtual ~PeoplezStringBuilderData() {delete[] replacings;}

			/**
			 * Sets all entries to empty strings
			 */
			void ResetReplacings();
			/**
			 * Setter for replacings
			 *
			 * Sets the value with that a replaceable in the source string should be replaced
			 *
			 * @param key The replaceable that should get a new value
			 * @param value New string to replace the replaceable part with
			 */
			void SetReplacing(PeoplezString const & key, PeoplezString const & value);
			/**
			 * Copy assignment operator
			 *
			 * @param rhs Instance that has to get copied
			 */
			PeoplezStringBuilderData & operator=(PeoplezStringBuilderData const & rhs);

		private:
			std::shared_ptr<Element> elements;
			/**
			 * Standard constructor
			 */
			PeoplezStringBuilderData() : len(0), replacings(0) {}
			/**
			 * Initializes the object with new replaceables
			 *
			 * @param Replaceables New replaceables
			 * @param Len Number of new replaceables
			 */
			void SetReplaceables(PeoplezString const *Replaceables, int Len);

			int len;
			boost::shared_array<PeoplezString> replaceables;
			PeoplezString* replacings;
		};
	} // namespace String
} // namespace Peoplez

#endif // PEOPLEZ_STRING_PEOPLEZSTRINGBUILDERDATA_H_
