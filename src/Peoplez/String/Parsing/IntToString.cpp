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
#include "IntToString.hpp"

// External includes
#include <algorithm>

namespace Peoplez
{
	namespace String
	{
		namespace Parsing
		{
			template<>
			unsigned int ToCStringBase10(char * target, int32_t const val_)
			{
				char * const begin = target;
				int64_t val = val_;

				if(val < 0)
				{
					*target++ = '-';
					val *= -1;
				}

				do
				{
					*target++ = '0' + val % 10;
					val /= 10;
				}while(val);

				std::reverse(val_ < 0 ? begin + 1 : begin, target);

				*target = '\0';

				return target - begin;
			}

			template<>
			unsigned int ToCStringBase10(char * target, uint32_t val)
			{
				char * const begin = target;

				do
				{
					*target++ = '0' + val % 10;
					val /= 10;
				}while(val);

				std::reverse(begin, target);

				*target = '\0';

				return target - begin;
			}

			template<>
			unsigned int ToCStringBase10(char * target, int64_t val_)
			{
				char * const begin = target;
				bool neg = false;

				if(val_ < 0)
				{
					*target++ = '-';
					val_ *= -1;
					neg = true;
				}

				uint64_t val = val_;

				do
				{
					*target++ = '0' + val % 10;
					val /= 10;
				}while(val);

				std::reverse(neg ? begin + 1 : begin, target);

				*target = '\0';

				return target - begin;
			}

			template<>
			unsigned int ToCStringBase10(char * target, uint64_t val)
			{
				char * const begin = target;

				do
				{
					*target++ = '0' + val % 10;
					val /= 10;
				}while(val);

				std::reverse(begin, target);

				*target = '\0';

				return target - begin;
			}
		} // namespace Parsing
	} // namespace String
} // namespace Peoplez
