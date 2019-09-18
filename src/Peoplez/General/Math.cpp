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

// Own headers
#include "Math.hpp"
#include <ctime>
#include <cstdlib>
#include <mutex>

namespace Peoplez
{
	namespace General
	{
		namespace Math
		{
			uint64_t Rnd64()
			{
				static std::once_flag flag;
				std::call_once(flag, []{srand(time(0));});
			#if RAND_MAX == INT_MAX
				return (((rand() << 16) | (rand() & 0xffff)) << 16) | (rand() & 0xffff);
			#elif RAND_MAX == 65535
				return (((uint64_t)rand() << 48) | ((uint64_t)(rand() & 0xfff) << 36) | (((uint64_t)rand() & 0xfff) << 24) | (((uint64_t)rand() & 0xfff) << 12) | ((uint64_t)rand() & 0xfff));
			#elif RAND_MAX > 65535
				return ((((uint64_t)rand() & 0x7fff) << 48) | ((uint64_t)(rand() & 0xfff) << 36) | (((uint64_t)rand() & 0xfff) << 24) | (((uint64_t)rand() & 0xfff) << 12) | ((uint64_t)rand() & 0xfff));
			#else
				#error "SRAND_MAX doesn't fit"
				return -1;
			#endif
				return -1;
			}
		} // namespace Math
	} // namespace General
} // namespace Peoplez
