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
#include "HttpRequest.hpp"

namespace Peoplez
{
	// Local namespaces
	using namespace String;

	namespace Services
	{
		namespace Http
		{
			void HttpRequest::Clean()
			{
				eTag = 0;
				rawUrl.clear();
				contentLength = -1;
				cookies.clear();
				headers.clear();
				httpMethod = HttpMethods::UNKNOWN;
				postParams.clear();
				userLanguages.Clear();
				//preferredLanguage = (Language) -1;
			}

			int HttpRequest::FindBestLanguage(std::map<String::PeoplezString, int> langMap, int const def)
			{
				int result = def;	//Set result to default
				double bestQ = 0;	//Best quality value (q corresponding to rfc2616)
				PeoplezString tmpStr;
				char const * sectionBegin = userLanguages.GetData();
				char const * end = sectionBegin + userLanguages.Length();

				//For every <language code, preference> pair ...
				for(char const * sectionEnd = (char const *) memchr(sectionBegin, ',', end - sectionBegin); sectionBegin < end; sectionEnd = sectionEnd < end ? (char const *) memchr(sectionEnd + 1, ',', end - (sectionEnd + 1)) : 0)
				{
					if(!sectionEnd) sectionEnd = end;

					//Position of the semicolon between language code and preference
					char const * posSemicolon = (char const *) memchr(sectionBegin, ';', sectionEnd - sectionBegin);
					if(!posSemicolon) posSemicolon = sectionEnd;

					//Copy language code to string
					tmpStr.Clear();
					tmpStr.Append(sectionBegin, posSemicolon - sectionBegin);
					tmpStr.ToLower();

					//search for the language code in the preferences
					std::map<PeoplezString, int>::const_iterator iter = langMap.find(tmpStr);

					if(iter != langMap.end()) //if found ...
					{
						//Set posSemicolon to the position of the = in "q=..." or 0 if not existing
						++posSemicolon;
						posSemicolon = posSemicolon < sectionEnd ? (char const *) memchr(posSemicolon, '=', sectionEnd - posSemicolon) : 0;

						if(posSemicolon) //if a preference for this language code exists (in the request)
						{
							//Parse the quality value
							double q = atof(posSemicolon + 1);

							if(q > bestQ) //If better than all before ...
							{
								bestQ = q;
								result = iter->second;
							}
						}
						else
						{
							bestQ = 1;
							result = iter->second;
						}

						//If the new best language code hat proirity == 1 then stop searching
						if(bestQ >= 1) break;
					}

					//if(sectionEnd >= end) break;

					//Set beginning of search next section
					sectionBegin = sectionEnd + 1;
				}

				return result;
			}

			PeoplezString HttpRequest::GetHeaderValue(PeoplezString const & name)
			{
				for(size_t i = 0; i < headers.size(); ++i)
				{
					if(headers[i].first == name) return headers[i].second;
				}

				return PeoplezString();
			}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
