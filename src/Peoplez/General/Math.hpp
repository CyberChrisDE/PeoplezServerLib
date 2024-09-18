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

#ifndef PEOPLEZ_GENERAL_MATH_H_
#define PEOPLEZ_GENERAL_MATH_H_

// External includes
#include <cstdint>
#include <cstddef>
#include <bit>

namespace Peoplez
{
	namespace General
	{
		/**
		 * Math
		 * @brief Container for several mathematical functionalities
		 */
		namespace Math
		{
			/**
			 * Creates a random uint64_t number
			 *
			 * @return Random number
			 */
			uint64_t Rnd64();
			/**
			 * Identifies the index of the LSB that is set
			 *
			 * LSB = Least significant Bit
			 * val == 0 causes undefined behavior
			 *
			 * @param val value
			 */
			constexpr unsigned int minBitSet(uint64_t const val) noexcept {return std::countr_zero(val);}
			/**
			 * @copydoc minBitSet(uint64_t val)
			 */
			constexpr unsigned int minBitSet(uint32_t const val) noexcept {return std::countr_zero(val);}
			/**
			 * Identifies the index of the MSB that is set
			 *
			 * MSB = Most significant Bit
			 * val == 0 causes undefined behavior
			 *
			 * @param val value
			 */
			constexpr unsigned int maxBitSet(uint64_t const val) noexcept {return 63 - std::countl_zero(val);}
			/**
			 * @copydoc maxBitSet(uint64_t val)
			 */
			constexpr unsigned int maxBitSet(uint32_t const val) noexcept {return 31 - std::countl_zero(val);}

#if __GNUC__ || __clang__
#define FAST_OVERFLOW_CHECKS
			template <typename T1, typename T2, typename T3>
			inline bool addOverflow(T1 const a, T2 const b, T3 * const c) {return __builtin_add_overflow(a, b, c);}

			template <typename T1, typename T2, typename T3>
			inline bool subOverflow(T1 const a, T2 const b, T3 * const c) {return __builtin_sub_overflow(a, b, c);}

			template <typename T1, typename T2, typename T3>
			inline bool mulOverflow(T1 const a, T2 const b, T3 * const c) {return __builtin_mul_overflow(a, b, c);}
#else
			template <typename T1, typename T2, typename T3>
			inline bool addOverflow(T1 const a, T2 const b, T3 * const c)
			{
				#error "Not implemented for this compiler"
			}

			template <typename T1, typename T2, typename T3>
			inline bool subOverflow(T1 const a, T2 const b, T3 * const c)
			{
				#error "Not implemented for this compiler"
			}

			template <typename T1, typename T2, typename T3>
			inline bool mulOverflow(T1 const a, T2 const b, T3 * const c)
			{
				#error "Not implemented for this compiler"
			}
#endif
		} // namespace Math
	} // namespace General
} // namespace Peoplez

#endif // PEOPLEZ_GENERAL_MATH_H_
