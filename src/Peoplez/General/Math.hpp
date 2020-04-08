/**
 * Copyright 2017, 2018 Christian Geldermann
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

#define CLZ_u32(x) ((x) ? __builtin_clz(x) : 32)
#define CLZ_u64(x) ((x) ? __builtin_clzl(x) : 64)
#define CTZ_u32(x) ((x) ? __builtin_ctz(x) : 32)
#define CTZ_u64(x) ((x) ? __builtin_ctzl(x) : 64)

#ifdef __LZCNT__
extern "C"
{
	#include <x86intrin.h>
}
#endif

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
			inline unsigned int minBitSet(unsigned long val) noexcept {return __builtin_ctzl(val);}
			/**
			 * @copydoc minBitSet(uint64_t val)
			 */
			inline unsigned int minBitSet(unsigned int val) noexcept {return __builtin_ctz(val);}
			/**
			 * Identifies the index of the MSB that is set
			 *
			 * MSB = Most significant Bit
			 * val == 0 causes undefined behavior
			 *
			 * @param val value
			 */
	#ifdef __LZCNT__
			inline unsigned int maxBitSet(uint64_t val) noexcept {return 63 - _lzcnt_u64(val);}
	#else
			//inline unsigned int maxBitSet(uint64_t val) noexcept {unsigned int res1 = __builtin_clz((uint32_t) (val >> 32)); return res1 < 32 ? 63 - res1 : 31 - __builtin_clz((uint32_t) val);}
			inline unsigned int maxBitSet(uint64_t val) noexcept {return 63 - CLZ_u64(val);}
	#endif
			/**
			 * @copydoc maxBitSet(uint64_t val)
			 */
	#ifdef __LZCNT__
			inline unsigned int maxBitSet(uint32_t val) noexcept {return 31 - _lzcnt_u32(val);}
	#else
			inline unsigned int maxBitSet(uint32_t val) noexcept {return 31 - CLZ_u32(val);}
	#endif
			/**
			 * Counts the number of 0s before first 1
			 *
			 * Counts the number of Bits that are more significant than the most significant one that is set
			 */
			inline size_t LeadingZeros(unsigned char val) noexcept {return CLZ_u32(val) - 24;}
			/**
			 * @copydoc LeadingZeros(unsigned char val)
			 */
			inline size_t LeadingZeros(uint32_t val) noexcept {return CLZ_u32(val);}
			/**
			 * @copydoc LeadingZeros(unsigned char val)
			 */
			inline size_t LeadingZeros(uint64_t val) noexcept {return CLZ_u64(val);}
			/**
			 * Counts the number of 1s before first 0
			 *
			 * Counts the number of Bits that are more significant than the most significant one that is not set
			 *
			 * @param val value
			 */
			inline size_t LeadingOnes(unsigned char val) noexcept {return CLZ_u32(val ^ 0xFF) - 24;}
			/**
			 * @copydoc LeadingOnes(unsigned char val)
			 */
			inline size_t LeadingOnes(uint32_t val) noexcept {return CLZ_u32(val ^ 0xFFFFFFFF);}
			/**
			 * @copydoc LeadingOnes(unsigned char val)
			 */
			inline size_t LeadingOnes(uint64_t val) noexcept {return LeadingZeros(val ^ 0xFFFFFFFFFFFFFFFF);}

#if __GNUC__ || __clang__
#define FAST_OVERFLOW_CHECKS
#endif

#if __GNUC__ || __clang__
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
