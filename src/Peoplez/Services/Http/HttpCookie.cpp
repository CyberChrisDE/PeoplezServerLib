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
#include "HttpCookie.hpp"

namespace Peoplez
{
	// Local namespaces
	using namespace String;

	namespace Services
	{
		namespace Http
		{
			PeoplezString HttpCookie::ToString() const
			{
				if(!Name.IsEmpty())
				{
					PeoplezString ret(o_ToStringSize());

					ret.Append("Set-Cookie: ", 12);
					ret += Name;
					ret.Append("=", 1);
					ret += Value;
					ret.Append("; Version=1", 11);

					if(!Comment.IsEmpty())
					{
						ret.Append("; Comment=\"", 11);
						ret += Comment;
						ret.Append("\"", 1);
					}
					if(MaxAge > -1)
					{
						ret.Append("; Max-Age=", 10);
						ret += PeoplezString::ParseDec(MaxAge);
					}
					if(!Path.IsEmpty())
					{
						ret.Append("; Path=", 7);
						ret += Path;
					}
					if(!Port.IsEmpty())
					{
						ret.Append("; Port=", 7);
						ret += Port;
					}
					if(Discard) ret.Append("; Discard", 9);
					if(Secure) ret.Append("; Secure", 8);

					return ret;
				}

				return PeoplezString();
			}

			size_t HttpCookie::o_ToStringSize() const
			{
				register size_t res = 24 + Name.Length() + Value.Length();
				if(!Comment.IsEmpty()) res += 12 + Comment.Length();
				if(MaxAge > -1) res += 11 + MaxAge / 10;
				if(!Path.IsEmpty()) res += 7 + Path.Length();
				if(!Port.IsEmpty()) res += 7 + Port.Length();
				if(Discard) res += 9;
				if(Secure) res += 8;

				return res;
			}
		} // namespace Http
	} // namespace Services
} // namesapace Peoplez
