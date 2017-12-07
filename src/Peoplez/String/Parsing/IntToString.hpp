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

#ifndef PEOPLEZ_STRING_PARSING_INTTOSTRING_HPP_
#define PEOPLEZ_STRING_PARSING_INTTOSTRING_HPP_

// External includes
#include <cstdint>
#include <cstdlib>
#include <type_traits>

namespace Peoplez
{
	namespace String
	{
		namespace Parsing
		{
			/**
			 * Converts an integer type to a CString
			 *
			 * @param target Target CString
			 * @param val Source value
			 *
			 * @return Length of the CString (without terminating '\0'); Minimum is 1
			 */
			template<typename T> unsigned int ToCStringBase10(char *target, T val)
			{
				static_assert(std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value, "Not supported type for conversion");
				return 0;
			}

			/**
			 * Converts an int32 to a CString
			 *
			 * @param target Target CString
			 * @param val Source value
			 *
			 * @result Length of the CString (without terminating '\0'); Minimum is 1
			 */
			template<> unsigned int ToCStringBase10(char * target, int32_t val) __attribute__((nonnull));
			/**
			 * Converts an uint32 to a CString
			 *
			 * @param target Target CString
			 * @param val Source value
			 *
			 * @result Length of the CString (without terminating '\0'); Minimum is 1
			 */
			template<> unsigned int ToCStringBase10(char * target, uint32_t val) __attribute__((nonnull));
			/**
			 * Converts an int64 to a CString
			 *
			 * @param target Target CString
			 * @param val Source value
			 *
			 * @result Length of the CString (without terminating '\0'); Minimum is 1
			 */
			template<> unsigned int ToCStringBase10(char * target, int64_t val) __attribute__((nonnull));
			/**
			 * Converts an uint64 to a CString
			 *
			 * @param target Target CString
			 * @param val Source value
			 *
			 * @result Length of the CString (without terminating '\0'); Minimum is 1
			 */
			template<> unsigned int ToCStringBase10(char * target, uint64_t val) __attribute__((nonnull));
		} // namespace Parsing
	} // namespace String
} // namespace Peoplez

#endif // PEOPLEZ_STRING_PARSING_INTTOSTRING_HPP_
