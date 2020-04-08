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

#ifndef PEOPLEZ_STRING_PARSING_STINGTOINT_HPP_
#define PEOPLEZ_STRING_PARSING_STINGTOINT_HPP_

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
			 * Converts a CString to an integral type
			 *
			 * @param str Source string
			 * @param len Length of source sting (without '\0' termination)
			 * @param base Base of the number system of the input (e.g. 10 for decimal numbers)
			 *
			 * @result integral represented in the source string
			 */
			template<class T> T ToInt(char const * str, size_t len, unsigned char base) noexcept(false)
			{
				static_assert(std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value /*|| std::is_same<T, int64_t>::value*/ || std::is_same<T, uint64_t>::value, "Not supported type for conversion");
			}
			/**
			 * Converts aCString to an int16_t
			 *
			 * @param str Source string
			 * @param len Length of source string (without '\0' termination)
			 * @param base Base of the number system of the input (e.g. 10 for decimal numbers)
			 * @param error Error handling variable (0 = no error; 1 = not a number; 2 = number too large; 3 = other error)
			 *
			 * return number represented in the source string
			 */
			template<> int16_t ToInt<int16_t>(char const * str, size_t len, unsigned char base) noexcept(false) __attribute__((nonnull));
			/**
			 * Converts aCString to an uint16_t
			 *
			 * @param str Source string
			 * @param len Length of source string (without '\0' termination)
			 * @param base Base of the number system of the input (e.g. 10 for decimal numbers)
			 * @param error Error handling variable (0 = no error; 1 = not a number; 2 = number too large; 3 = other error)
			 *
			 * return number represented in the source string
			 */
			template<> uint16_t ToInt<uint16_t>(char const * str, size_t len, unsigned char base) noexcept(false) __attribute__((nonnull));
			/**
			 * Converts a CString to an int32_t
			 *
			 * @param str Source string
			 * @param len Length of source string (without '\0' termination)
			 * @param base Base of the number system of the input (e.g. 10 for decimal numbers)
			 * @param error Error handling variable (0 = no error; 1 = not a number; 2 = number too large; 3 = other error)
			 *
			 * @result number represented in the source string
			 */
			template<> int32_t ToInt<int32_t>(char const * str, size_t len, unsigned char base) noexcept(false) __attribute__((nonnull));
			/**
			 * Converts a CString to an uint32_t
			 *
			 * @param str Source string
			 * @param len Length of source string (without '\0' termination)
			 * @param base Base of the number system of the input (e.g. 10 for decimal numbers)
			 * @param error Error handling variable (0 = no error; 1 = not a number; 2 = number too large; 3 = other error)
			 *
			 * @result number represented in the source string
			 */
			template<> uint32_t ToInt<uint32_t>(char const * str, size_t len, unsigned char base) noexcept(false) __attribute__((nonnull));
			/**
			 * Converts a CString to an int64_t
			 *
			 * @param str Source string
			 * @param len Length of source string (without '\0' termination)
			 * @param base Base of the number system of the input (e.g. 10 for decimal numbers)
			 * @param error Error handling variable (0 = no error; 1 = not a number; 2 = number too large; 3 = other error)
			 *
			 * @result number represented in the source string
			 */
			template<> int64_t ToInt<int64_t>(char const * str, size_t len, unsigned char base) noexcept(false) __attribute__((nonnull));
			/**
			 * Converts a CString to an uint64_t
			 *
			 * @param str Source string
			 * @param len Length of source string (without '\0' termination)
			 * @param base Base of the number system of the input (e.g. 10 for decimal numbers)
			 * @param error Error handling variable (0 = no error; 1 = not a number; 2 = number too large; 3 = other error)
			 *
			 * @result number represented in the source string
			 */
			template<> uint64_t ToInt<uint64_t>(char const * str, size_t len, unsigned char base) noexcept(false) __attribute__((nonnull));
		} // namespace Parsing
	} // namespace String
} // namespace Peoplez

#endif // PEOPLEZ_STRING_PARSING_STINGTOINT_HPP_
