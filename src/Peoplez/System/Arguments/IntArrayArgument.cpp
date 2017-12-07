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

// Own headers
#include "IntArrayArgument.hpp"

// Local includes
#include "../../String/Parsing/StringToInt.hpp"


// External includes
#include <cstring>
#include <stdexcept>

// Local namespaces
using namespace Peoplez::String::Parsing;

namespace Peoplez
{
	namespace System
	{
		namespace Arguments
		{
	#define READ(type)																		\
			template<>																		\
			int IntArrayArgument<type, 1>::Read(int argc, char const * const argv[]) noexcept(false)	\
			{																				\
				int i = 0;																	\
				size_t len = 0;																\
																							\
				/*Throw empty entries away*/												\
				for(; i < argc; ++i)														\
				{																			\
					len = strlen(argv[i]);													\
					if(len) break;															\
				}																			\
																							\
				/*Read entry*/																\
				if(len && i < argc)															\
				{																			\
					try																		\
					{																		\
						type const result = ToInt<type>(argv[i], len, m_base);				\
						m_value = result;													\
						Argument::m_isSet = true;											\
					}																		\
					catch(std::exception & e)												\
					{																		\
						throw std::runtime_error(e.what());									\
					}																		\
				}																			\
																							\
				/*Return number of*/														\
				return std::min(argc, i + 1);												\
			}

			READ(int16_t);
			READ(uint16_t);
			READ(int32_t);
			READ(uint32_t);
			READ(int64_t);
			READ(uint64_t);
		} // namespace Arguments
	} // namespace System
} // namespace Peoplez
