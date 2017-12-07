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

// Own header
#include "StringToInt.hpp"

#include <stdexcept>

namespace Peoplez
{
	// External namespaces
	using namespace std;

	namespace String
	{
		namespace Parsing
		{
			template<>
			int16_t ToInt<int16_t>(char const * str, size_t const len, unsigned char const base) noexcept(false)
			{
				// Init
				char const * const end = str + len;
				int32_t result = 0;
				bool negative = false;

				if(len)
				{
					// Determine sign
					switch(*str)
					{
					case '+':
						++str;
						break;
					case '-':
						negative = true;
						++str;
						break;
					}

					// Compute absolute value
					if(base >= 2 && base <= 36) // If base is valid ...
					{
						for(; str < end; ++str) // For every char ...
						{
							// Init
							char const charVal = *str;
							uint64_t numVal = 0;

							if(charVal >= 48 && charVal <= 57) numVal = charVal - 48;
							else if(charVal >= 65 && charVal <= 90) numVal = charVal - 55;
							else if(charVal >= 97 && charVal <= 122) numVal = charVal - 87;
							else throw runtime_error("The string does not represent a number for the given base");

							if(__builtin_expect(numVal >= base, false)) throw runtime_error("The string does not represent a number for the given base ");

							result = result * base + numVal;

							// Error for absolute overflow
							if(__builtin_expect(result > 0x8000, false)) throw overflow_error("The absolute number represented by the string is too large");
						}
					}
					else throw out_of_range("base must be in [2, 36]");
				}

				if(negative) return (int16_t) -result;
				else
				{
					if(result & 0x8000) throw overflow_error("The number represented by the string is too large");

					return (int16_t) result;
				}
			}

			template<>
			uint16_t ToInt<uint16_t>(char const * str, size_t const len, unsigned char const base) noexcept(false)
			{
				uint64_t result = 0;

				if(base >= 2 && base <= 36)
				{
					char const * const end = str + len;

					if(len && (*str == '+')) ++str;

					for(; str < end; ++str)
					{
						char const charVal = *str;
						uint32_t numVal = 0;

						if(charVal >= 48 && charVal <= 57) numVal = charVal - 48;
						else if(charVal >= 65 && charVal <= 90) numVal = charVal - 55;
						else if(charVal >= 97 && charVal <= 122) numVal = charVal - 87;
						else throw runtime_error("The string does not represent a number for the given base");

						if(__builtin_expect(numVal >= base, false)) throw runtime_error("The string does not represent a number for the given base");

						result = result * base + numVal;

						if(__builtin_expect(result > 0xFFFF, false)) throw overflow_error("The number represented by the string is too large");
					}
				}
				else throw out_of_range("base must be in [2, 36]");

				return (uint16_t) result;
			}

			template<>
			int32_t ToInt<int32_t>(char const * str, size_t const len, unsigned char const base) noexcept(false)
			{
				char const * const end = str + len;
				int64_t result = 0;
				bool negative = false;

				if(len)
				{
					switch(*str)
					{
					case '+':
						++str;
						break;
					case '-':
						negative = true;
						++str;
						break;
					}

					if(base >= 2 && base <= 36)
					{
						for(; str < end; ++str)
						{
							char const charVal = *str;
							uint64_t numVal = 0;

							if(charVal >= 48 && charVal <= 57) numVal = charVal - 48;
							else if(charVal >= 65 && charVal <= 90) numVal = charVal - 55;
							else if(charVal >= 97 && charVal <= 122) numVal = charVal - 87;
							else throw runtime_error("The string does not represent a number for the given base");

							if(__builtin_expect(numVal >= base, false)) throw runtime_error("The string does not represent a number for the given base");

							result = result * base + numVal;

							if(__builtin_expect(result > 0x80000000, false)) throw overflow_error("The absolute number represented by the string is too large");
						}
					}
					else throw out_of_range("base must be in [2, 36]");
				}

				if(negative) return (int32_t) -result;
				else
				{
					if(result & 0x80000000) throw overflow_error("The number represented by the string is too large");

					return (int32_t) result;
				}
			}

			template<>
			uint32_t ToInt<uint32_t>(char const * str, size_t const len, unsigned char const base) noexcept(false)
			{
				uint64_t result = 0;

				if(base >= 2 && base <= 36)
				{
					char const * const end = str + len;

					if(len && (*str == '+')) ++str;

					for(; str < end; ++str)
					{
						char const charVal = *str;
						uint64_t numVal = 0;

						if(charVal >= 48 && charVal <= 57) numVal = charVal - 48;
						else if(charVal >= 65 && charVal <= 90) numVal = charVal - 55;
						else if(charVal >= 97 && charVal <= 122) numVal = charVal - 87;
						else throw runtime_error("The string does not represent a number for the given base");

						if(__builtin_expect(numVal >= base, false)) throw runtime_error("The string does not represent a number for the given base");

						result = result * base + numVal;

						if(__builtin_expect(result > 0xFFFFFFFF, false)) throw overflow_error("The number represented by the string is too large");
					}
				}
				else throw out_of_range("base must be in [2, 36]");

				return result;
			}

			template<>
			uint64_t ToInt<uint64_t>(char const * str, size_t const len, unsigned char const base) noexcept(false)
			{
				if(base >= 2 && base <= 36)
				{
					uint64_t upper = 0, lower = 0;
					char const * const end = str + len;

					if(len && (*str == '+')) ++str;

					for(; str < end; ++str)
					{
						char const charVal = *str;
						uint64_t numVal = 0;

						if(charVal >= 48 && charVal <= 57) numVal = charVal - 48;
						else if(charVal >= 65 && charVal <= 90) numVal = charVal - 55;
						else if(charVal >= 97 && charVal <= 122) numVal = charVal - 87;
						else throw runtime_error("The string does not represent a number for the given base");

						if(__builtin_expect(numVal >= base, false)) throw runtime_error("The string does not represent a number for the given base");

						lower = lower * base + numVal;
						upper = upper * base + (lower >> 32);
						lower &= 0xFFFFFFFF;

						if(__builtin_expect(upper > 0xFFFFFFFF, false)) throw overflow_error("The number represented by the string is too large");
					}

					return (upper << 32) + lower;
				}
				else throw out_of_range("base must be in [2, 36]");
			}

			template<>
			int64_t ToInt<int64_t>(char const * str, size_t len, unsigned char const base) noexcept(false)
			{
				uint64_t result = 0;
				int64_t negative = false;

				if(len)
				{
					//check for sign
					switch(str[0])
					{
					case '-':
						negative = true;
						++str;
						--len;
						break;
					case '+':
						++str;
						--len;
						break;
					}

					result = ToInt<uint64_t>(str, len, base);

					//check for overflow and return result if no overflow
					if(negative)
					{
						if(result <= 0x8000000000000000) return -(int64_t) result;
						else throw overflow_error("The absolute number represented by the string is too large");
					}
					else
					{
						if(result < 0x8000000000000000) return (int64_t) result;
						else throw overflow_error("The absolute number represented by the string is too large");
					}
				}

				return 0;
			}
		} // namespace Parsing
	} // namespace String
} // namespace Peoplez
