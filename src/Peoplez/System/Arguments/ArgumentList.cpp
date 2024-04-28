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
#include "ArgumentList.hpp"

// External includes
#include <cstring>
#include <stdexcept>

namespace Peoplez
{
	namespace System
	{
		namespace Arguments
		{
			void ArgumentList::AddArgument(Argument * const arg)
			{
				m_named_args.push_back(arg);

				std::vector<std::string> const & names = arg->GetNames();

				for(std::vector<std::string>::const_iterator nameIter = names.begin(); nameIter != names.end(); ++nameIter)
					m_name_map.insert(std::pair<std::string, Argument *>(*nameIter, arg));
			}

			void ArgumentList::ReadArguments(int const argc, char const * const argv[])
			{
				m_program_name = argv[0];

				for(int i = 1; i < argc;)
				{
					size_t const size = strlen(argv[i]);

					if(size)
					{
						if(argv[i][0] == '-')
						{
							std::string name(argv[i] + 1);

							std::map<std::string, Argument *>::const_iterator const param = m_name_map.find(name);

							if(param != m_name_map.end())
							{
								i += param->second->Read(argc - i - 1, argv + i + 1) + 1;
							}
							else
							{
								//std::string error(name);
								name += " is not a known argument";
								throw std::runtime_error(name.c_str());
							}
						}
						else
						{
							m_unnamed_args.push_back(argv[i]);
							++i;
						}
					}
					else ++i;
				}
			}

			Argument * ArgumentList::GetNamedArgument(std::string const name)
			{
				std::map<std::string, Argument *>::const_iterator const iter = m_name_map.find(name);

				if(iter != m_name_map.end() && iter->second != 0) return iter->second;
				else return 0;
			}
		} // namespace Parameters
	} // namespace System
} // namespace General
