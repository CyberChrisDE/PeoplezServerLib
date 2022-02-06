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

// Local includes
#include "PeoplezStringBuilderData.hpp"

namespace Peoplez
{
	namespace String
	{
		PeoplezStringBuilderData::PeoplezStringBuilderData(const PeoplezStringBuilderData &original) : elements(original.elements), len(original.len), replaceables(original.replaceables), replacings(len ? new PeoplezString[len] : 0)
		{
			for(int i = 0; i < len; ++i)
				if(!original.replaceables[i].IsEmpty())
					replacings[i] = original.replacings[i].UniqueCopy();
		}

		void PeoplezStringBuilderData::ResetReplacings()
		{
			for(int i = 0; i < len; ++i) replacings[i].Clear();
		}

		void PeoplezStringBuilderData::SetReplacing(PeoplezString const &key, PeoplezString const &value)
		{
			for(int i = 0; i < len; ++i)
			{
				if(replaceables[i] == key)
				{
					replacings[i] = value;
					break;
				}
			}
		}

		void PeoplezStringBuilderData::SetReplaceables(PeoplezString const *Replaceables, int Len)
		{
			len = Len;
			replaceables = std::shared_ptr<PeoplezString[]>(new PeoplezString[Len], std::default_delete<PeoplezString[]>());
			for(int i = 0; i < len; ++i) replaceables[i] = Replaceables[i];
			delete[] replacings;
			replacings = new PeoplezString[Len];
		}

		PeoplezStringBuilderData &PeoplezStringBuilderData::operator=(PeoplezStringBuilderData const &rhs)
		{
			elements = rhs.elements;
			len = rhs.len;
			replaceables = rhs.replaceables;
			delete [] replacings;
			replacings = len ? new PeoplezString[len] : 0;
			for(int i = 0; i < len; ++i) replacings[i] = rhs.replacings[i];

			return *this;
		}
	} // namespace String
} // namespace Peoplez
