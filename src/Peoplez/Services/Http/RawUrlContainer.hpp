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

#ifndef PEOPLEZ_SERVICES_HTTP_RAWURLCONTAINER_H_
#define PEOPLEZ_SERVICES_HTTP_RAWURLCONTAINER_H_

// Local includes
#include "../../String/PeoplezString.hpp"

// Extern includes
#include <cstdint>

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			/**
			 * @brief Container for easier navigation in raw urls
			 */
			class RawUrlContainer final
			{
			public:
				RawUrlContainer() : startIndex(0) {}
				virtual ~RawUrlContainer() {}

				/**
				 * Subscription operator
				 *
				 * @param index Index relative to the actual position
				 */
				inline String::PeoplezString operator [] (uint64_t index) {return Get(index);}

				/**
				 * Resets all members
				 *
				 * Deletes the raw url and resets the actual position to 0
				 */
				inline void Clear() {rawUrl.clear(); startIndex = 0;}
				/**
				 * Getter for specific url part
				 *
				 * @param index Index relative to the actual position
				 */
				inline String::PeoplezString Get(uint64_t index = 0) {return rawUrl[index + startIndex];}
				/**
				 * Getter for specific url part in lower case
				 *
				 * @param index Index relative to the actual position
				 */
				String::PeoplezString GetToLower(uint64_t index = 0);
				/**
				 * Getter for specific url part in upper case
				 *
				 * @param index Index relative to the actual position
				 */
				String::PeoplezString GetToUpper(uint64_t index = 0);
				/**
				 * Setter for actual position
				 *
				 * @param steps New actual position
				 */
				inline void MoveIndex(uint64_t steps) {startIndex += steps;}

			private:
				uint64_t startIndex;
				std::vector<String::PeoplezString> rawUrl;
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_RAWURLCONTAINER_H_
