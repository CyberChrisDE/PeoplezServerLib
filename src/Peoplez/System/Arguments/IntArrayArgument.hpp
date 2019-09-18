/**
 * Copyright 2017, 2019 Christian Geldermann
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

#ifndef PEOPLEZ_SYSTEM_ARGUMENTS_INTARRAYARGUMENT_HPP_
#define PEOPLEZ_SYSTEM_ARGUMENTS_INTARRAYARGUMENT_HPP_

// Local includes
#include "Argument.hpp"

// External includes
#include <type_traits>
#include <iostream>

namespace Peoplez
{
	namespace System
	{
		namespace Arguments
		{
			/**
			 * @brief Argument with fixed numbr of integer values
			 */
			template<typename T, unsigned char NINT>
			class IntArrayArgument final : public Argument
			{
				static_assert(std::is_same<T, int16_t>::value | std::is_same<T, uint16_t>::value | std::is_same<T, int32_t>::value | std::is_same<T, uint32_t>::value | std::is_same<T, int64_t>::value | std::is_same<T, uint64_t>::value, "class IntArrayArgument can only be instantiated with 16/32/64 bit (unsigned) integer types");
			public:
				/**
				 * Constructor
				 *
				 * @param names Names/Identifiers/Keys of the argument
				 * @param base Base of the number system of the input (default is 10)
				 */
				IntArrayArgument(std::vector<std::string> names, unsigned char base = 10) : Argument(names), m_base(base) {}
				virtual ~IntArrayArgument() {}

				virtual int Read(int argc, char const * const argv[]) = 0;
			private:
				int32_t m_values[NINT];
				char m_base;
			};

			/**
			 * @brief Argument with a single int value
			 */
			template<typename T>
			class IntArrayArgument<T, 1> final : public Argument
			{
			public:
				/**
				 * Constructor
				 *
				 * @param names Names/Identifiers/Keys of the argument
				 * @param base Base of the number system of the input (default is 10)
				 */
				IntArrayArgument(std::vector<std::string> names, unsigned char base = 10) : Argument(names), m_value(0), m_base(base) {}
				virtual ~IntArrayArgument() {}

				virtual int Read(int argc, char const * const argv[]) noexcept(false);
				/**
				 * Getter for the value
				 */
				T GetValue() const {return m_value;}

			private:
				T m_value;
				unsigned char m_base = 10;
			};
		} // namespace Arguments
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_ARGUMENTS_INTARRAYARGUMENT_HPP_
