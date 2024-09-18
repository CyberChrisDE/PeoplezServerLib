/**
 * Copyright 2017, 2018, 2024 Christian Geldermann
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

namespace Peoplez
{
	namespace String
	{
		namespace Parsing
		{
			/**
			 * Converts an int32 to a CString
			 *
			 * @param target Target CString.
			 * This should have at least 12 bytes reserved (10 digits + sign + '\0'-termination)
			 * @param val Source value
			 *
			 * @result Length of the CString (without terminating '\0');
			 */
			unsigned int ToCStringBase10(char * target, int32_t val) noexcept __attribute__((nonnull));
			//@ requires chars(target, ?count, _) &*& count >= 12;
			//@ ensures chars(target, count, _) &*& result <= 11;
			/**
			 * Converts an uint32 to a CString
			 *
			 * @param target Target CString.
			 * This should have at least 11 bytes reserved (10 digits + '\0'-termination)
			 * @param val Source value
			 *
			 * @result Length of the CString (without terminating '\0');
			 */
			unsigned int ToCStringBase10(char * target, uint32_t val) noexcept __attribute__((nonnull));
			//@ requires chars(target, ?count, _) &*& count >= 11;
			//@ ensures chars(target, count, _) &*& result <= 10;
			/**
			 * Converts an int64 to a CString
			 *
			 * @param target Target CString
			 * This should have at least 21 bytes reserved (19 digits + sign + '\0'-termination)
			 * @param val Source value
			 *
			 * @result Length of the CString (without terminating '\0');
			 */
			unsigned int ToCStringBase10(char * target, int64_t val) noexcept __attribute__((nonnull));
			//@ requires chars(target, ?count, _) &*& count >= 22;
			//@ ensures chars(target, count, _) &*& result <= 21;
			/**
			 * Converts an uint64 to a CString
			 *
			 * @param target Target CString
			 * This should have at least 21 bytes reserved (20 digits + '\0'-termination)
			 * @param val Source value
			 *
			 * @result Length of the CString (without terminating '\0');
			 */
			unsigned int ToCStringBase10(char * target, uint64_t val) noexcept __attribute__((nonnull));
			//@ requires chars(target, ?count, _) &*& count >= 21;
			//@ ensures chars(target, count, _) &*& result <= 20;
		} // namespace Parsing
	} // namespace String
} // namespace Peoplez

#endif // PEOPLEZ_STRING_PARSING_INTTOSTRING_HPP_
