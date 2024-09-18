/**
 * Copyright 2017, 2019, 2024 Christian Geldermann
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

// Own header
// #include "IntToString.hpp"

// External includes
#include <cstdint>
#include <algorithm>

//@ #include <nat.gh>

//@ fixpoint nat N19() { return succ(succ(succ(succ(N15)))); }
//@ fixpoint nat N20() { return succ(N19); }

namespace Peoplez
{
	namespace String
	{
		namespace Parsing
		{
			unsigned int ToCStringBase10(char * target, uint32_t val) noexcept
			//@ requires chars(target, ?count, _) &*& count >= 11;
			//@ ensures chars(target, count, _) &*& result <= 10;
			{
				//@ chars_split(target, 0);
				unsigned i = 0;
				//@ nat p = N10;

				// Decode decimal digits (in reverse order)
				do
				//@ invariant chars(target, i, _) &*& chars(target + i, count - i, _) &*& p != zero &*& i == 10 - int_of_nat(p) &*& val < pow_nat(10, p);
				{
					//@ div_rem_nonneg(val, 10);
					char const c = (char)('0' + (char)(val % 10));
					//@ open chars(target + i, count - i, _);
					//@ character_limits(target + i);
					*(target + i) = c;
					//@ close chars(target + i, 1, _);
					//@ chars_join(target);
					//@ assert p == succ(_);
					val /= 10;
					//@ p = nat_predecessor(p);
					//@ assert (val <= 0 && val >= 0) ? val == 0 : true;
					++i;
				}while(val);

				// Reverse order of digits to correct order
				std::reverse(target, target + i);

				// Add '\0'-termination
				//@ open chars(target + i, count - i, _);
				*(target + i) = '\0';				
				//@ close chars(target + i, count - i, _);
				//@ chars_join(target);

				// Return length of string
				return i;
			}

			unsigned int ToCStringBase10(char * target, int32_t const val) noexcept
			//@ requires chars(target, ?count, _) &*& count >= 12;
			//@ ensures chars(target, count, _) &*& result <= 11;
			{
				uint32_t val_;

				if(val < 0)
				{
					val_ = (val == -2147483648) ? (uint32_t) 2147483648U : (uint32_t) -val;
					//@ open chars(target, count, _);
					//@ open chars(target + 1, count - 1, _);
					//@ character_limits(target + 1);
					*target++ = '-';
					//@ close chars(target, count - 1, _);
				}
				else val_ = (uint32_t) val;

				return ToCStringBase10(target, val_);
			}

			unsigned int ToCStringBase10(char * target, uint64_t val) noexcept
			//@ requires chars(target, ?count, _) &*& count >= 21;
			//@ ensures chars(target, count, _) &*& result <= 20;
			{
				//@ chars_split(target, 0);
				unsigned i = 0;
				//@ nat p = N20;

				// Decode decimal digits (in reverse order)
				do
				//@ invariant chars(target, i, _) &*& chars(target + i, count - i, _) &*& p != zero &*& i == 20 - int_of_nat(p) &*& val < pow_nat(10, p);
				{
					//@ div_rem_nonneg(val, 10);
					char const c = (char)('0' + (char)(val % 10));
					//@ open chars(target + i, count - i, _);
					//@ character_limits(target + i);
					*(target + i) = c;
					//@ close chars(target + i, 1, _);
					//@ chars_join(target);
					//@ assert p == succ(_);
					val /= 10;
					//@ p = nat_predecessor(p);
					//@ assert (val <= 0 && val >= 0) ? val == 0 : true;
					++i;
				}while(val);

				// Reverse order of digits to correct order
				std::reverse(target, target + i);

				// Add '\0'-termination
				//@ open chars(target + i, count - i, _);
				*(target + i) = '\0';				
				//@ close chars(target + i, count - i, _);
				//@ chars_join(target);

				// Return length of string
				return i;
			}

			unsigned int ToCStringBase10(char * target, int64_t const val) noexcept
			//@ requires chars(target, ?count, _) &*& count >= 22;
			//@ ensures chars(target, count, _) &*& result <= 21;
			{
				uint64_t val_;

				if(val < 0)
				{
					val_ = (val == -9223372036854775808LL) ? (uint64_t) 9223372036854775808ULL : (uint64_t) -val;
					//@ open chars(target, count, _);
					//@ open chars(target + 1, count - 1, _);
					//@ character_limits(target + 1);
					*target++ = '-';
					//@ close chars(target, count - 1, _);
				}
				else val_ = (uint64_t) val;

				return ToCStringBase10(target, val_);
			}
		} // namespace Parsing
	} // namespace String
} // namespace Peoplez
