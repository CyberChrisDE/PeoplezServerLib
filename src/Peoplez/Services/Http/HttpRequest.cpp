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
				//rawUrl.clear();
				contentLength = -1;
				cookies.clear();
				headers.clear();
				httpMethod = HttpMethods::UNKNOWN;
				postParams.clear();
				userLanguages.Clear();
				//preferredLanguage = (Language) -1;
				uri.Clean();
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

			HttpRequestUri::HttpRequestUri(String::PeoplezString uriString, HttpMethods const httpMethod) :
					scheme(uriString.Substring(0,0)), authorityString(scheme), pathString(scheme), queryString(scheme)
			{
				if(httpMethod == HttpMethods::CONNECT)
				{
					// According to RFC 7231 Section 4.3.6 only the
					// authority-form (RFC 7230 Section 5.3.3) is allowed
					// for method CONNECT.
					// According to RFC 7230 Section 5.3.3 the authority-form
					// is only used for CONNECT requests and thus only checked
					// here. And RFC 7230 Section 5.3.3 disallows the use of
					// the userinfo.

					//TODO: No syntax check done yet
					authorityString = uriString;
				}
				else if(uriString.EqualTo("*", 1))
				{
					// Asterisk-form (RFC 7230 Section 5.3.4) is only
					// possible for method OPTIONS (and possibly CONNECT).
					// If it is not allowed, type is set to UNDEFINED anyway
					// as UNDEFINED is the default
					if(httpMethod == HttpMethods::OPTIONS) type = UriType::ASTERISK;
				}
				else // Non-CONNECT and no asterik-form ...
				{
					// Extract query
					{
						size_t const posQM = uriString.Find('?');
						if(posQM != PeoplezString::NPOS)
						{
							queryString = uriString.Substring(posQM + 1);
							uriString = uriString.Substring(0, posQM);
						}
					}

					// From now on, only the origin-form (RFC 7230 Section 5.3.1)
					// and the absolute-form (RFC 7230 Section 5.3.2) are possible.
					// The origin-form must begin with a '/' and the absolute-form
					// must not.

					size_t const posSlash = uriString.Find('/');
					if(posSlash == 0) // Origin-form
					{
						type = UriType::ORIGIN;
						pathString = uriString;
						// Split the path into its segments
						uriString.Split<true>(pathSegments, '/');
					}
					else // Absolute-form
					{
						size_t const posColon = uriString.Find(':');
						if(posColon != PeoplezString::NPOS)
						{
							type = UriType::ABSOLUTE;
							// Extract the scheme
							scheme = uriString.Substring(0, posColon);
							// and remove it from uriString
							uriString = uriString.Substring(posColon + 1);
							// Remaining uriString is the hier-part (see RFC 3986 Section 3)

							if(uriString.Length() >= 2 && uriString[0] != '/' && uriString[1] != '/')
							{
								// uriString is: "//" authority path-abempty
								// First find the '/' and extract the authority
								// Then reduce uriString to path
								size_t const posSlash = uriString.Find('/', 2);

								if(posSlash == PeoplezString::NPOS)
								{
									authorityString = uriString.Substring(2);
									// Path is empty
									uriString = uriString.Substring(0,0);
								}
								else
								{
									authorityString = uriString.Substring(2, posSlash - 2);
									uriString = uriString.Substring(posSlash + 1);
								}

								// Split path into its segments
								pathString = uriString;
								if(!uriString.IsEmpty()) uriString.Split<true>(pathSegments, '/');
							}

							// Remaining uriString is the path

							pathString = uriString;
							// Split the path into its segments
							uriString.Split<true>(pathSegments, '/');
						}
						else
						{
							// Reset the query string (to empty string)
							queryString = uriString.Substring(0,0);

							// type = UNDEFINED is the default anyway
						}
					}

					// Unescape all path segments
					for(size_t i = 0; i < pathSegments.size(); ++i)
					{
						if(!pathSegments[i].DecodeUrl())
						{
							// If decoding fails
							// reset all fields
							type = UriType::UNDEFINED;
							if(!scheme.IsEmpty()) scheme = uriString.Substring(0,0);
							if(!authorityString.IsEmpty()) authorityString = scheme;
							if(!pathString.IsEmpty()) pathString = scheme;
							if(!queryString.IsEmpty()) queryString = scheme;
							pathSegments.clear();

							// Stop decoding
							break;
						}
					}
				}
			}

			void HttpRequestUri::Clean()
			{
				type = UriType::UNDEFINED;
				scheme.Clear();
				authorityString.Clear();
				pathString.Clear();
				pathSegments.clear();
				queryString.Clear();
			}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
