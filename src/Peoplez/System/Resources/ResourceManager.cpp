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
#include "ResourceManager.hpp"

// Local includes
#include "../../System/Logging/Logger.hpp"
#include "ResourceHolderPreloaded.hpp"

// External includes
#include <fstream>
#include <cstdio>
#include <cstdlib>
extern "C"
{
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
}

// External namespaces
using namespace std;

namespace Peoplez
{
	// Local namespaces
	using namespace Services::Http;
	using namespace String;

	namespace System
	{
		using namespace Logging;

		namespace Resources
		{
			void ResourceManager::Initialize() noexcept
			{
				try
				{
					{//Loading public files
						struct stat fileInfo;
						struct dirent *currentFile;
						DIR *dir;

						if((dir = opendir(directory.EnsureZeroTermination().GetData())) == NULL) Logger::LogException("Public directory could not be opened", __FILE__, __LINE__);
						else
						{
							while((currentFile = readdir(dir)) != NULL)
							{
								if(currentFile->d_name[0] != '.' && currentFile->d_name[0] != '\0')
								{
									PeoplezString const fileName(currentFile->d_name);

									if(stat((directory + fileName).EnsureZeroTermination().GetData(), &fileInfo) >= 0 && S_ISREG(fileInfo.st_mode))
									{
										if(((S_IXUSR | S_IXGRP | S_IXOTH) & fileInfo.st_mode) != 0) Logger::LogEvent("!!!Executable in public directory found!!!");
										else GetResource(fileName, 0);
									}
								}
							}
						}

						closedir(dir);
					}

					Logger::LogEvent("ResourceManager initialized");
				}
				catch (...)
				{
					Logger::LogException("Error in ResourceManager::Initialize", __FILE__, __LINE__);
				}
			}

			Resource ResourceManager::GetResource(String::PeoplezString fileName, size_t hash)
			{
				try
				{
					std::unique_lock<std::mutex> lock(resourceMutex);

					int first = -1, last = resources.size();

					while(first + 1 < last)
					{
						int const pos = ((first + last) >> 1);
						int const comparison = resources[pos].Name.Compare(fileName);

						if(comparison > 0) first = pos;
						else if(comparison < 0) last = pos;
						else return resources[pos].GetResource(hash);
					}

					ResourceHolderPreloaded *resource = new ResourceHolderPreloaded(directory, fileName);
					resources.insert(resources.begin() + last, resource);

					return resource->GetResource(hash);
				}
				catch (...)
				{
					Logger::LogException("Error in ResourceManger::AddFile", __FILE__, __LINE__);
				}

				return Resource();
			}

			/*
			FileSaveStatus ResourceManager::SaveFile(uint64_t userID, PeoplezString fileName, PeoplezString content) noexcept
			{
				try
				{
					struct stat buf;
					PeoplezString completeFileName = UserDataDirectory + PeoplezString::ParseDec(userID) + fileName;
					completeFileName.EnsureZeroTermination();

					if(stat(completeFileName.GetData(), &buf) == -1)
					{
						ofstream file(completeFileName.GetData(), ios::out | ios::binary);

						file.write(content.GetData(), content.Length());
						file.close();

						return FILE_SAVE_STATUS_OK;
					}
					else return FILE_SAVE_STATUS_DUPLICATE;
				}
				catch (...)
				{
					Logger::LogException("Error in ResourceManager::SaveFile", __FILE__, __LINE__);
				}

				return FILE_SAVE_STATUS_ERROR;
			}
			*/

			ResourceManager::~ResourceManager() noexcept
			{
				Logger::LogEvent("ResourceManager closed");
			}
		} // namespace Resources
	} // namespace System
} // namespace Peoplez
