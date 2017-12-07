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
#include "ResourceHolderPreloaded.hpp"

// Local includes
#include "../../General/FileOperations.hpp"
#include "../../System/Logging/Logger.hpp"

// Extern includes
#include <sys/stat.h>
#include <fstream>

// External namespaces
using namespace std;

namespace Peoplez
{
	// Local namespaces
	using namespace String;
	using namespace General;

	namespace System
	{
		using namespace Logging;

		namespace Resources
		{
			ResourceHolderPreloaded::ResourceHolderPreloaded(PeoplezString directory, PeoplezString fileName) : ResourceHolder(fileName), compressed(false), Path(directory + fileName), type(FILE_TYPE::NONE)
			{
				Path.EnsureZeroTermination();
				size_t dotPosition = fileName.FindLast('.');
				if(dotPosition != PeoplezString::NPOS) type = FileOperations::FileExtensionToFileType(fileName.Substring(dotPosition + 1));
				compressed = IsCategoryOf(type, FILE_TYPE::TEXT);
			}

			Resource ResourceHolderPreloaded::GetResource(size_t hashValue) noexcept(noexcept(Resource(false, "", RESOURCE_STATUS_ERROR, 0, FILE_TYPE::NONE)))
			{
				try
				{
					boost::unique_lock<boost::mutex> lock(contentMutex);

					if(NeedsSetup())
					{
						lastSetup = time(0);

						struct stat fileInfo;
						stat(Path.GetData(), &fileInfo);

						if(fileInfo.st_mtime != lastModified)
						{
							lastModified = fileInfo.st_mtime;
							off_t size = fileInfo.st_size;
							char *data = new char[size];

							ifstream file(Path.GetData(), ios::in | ios::binary);

							if(file.is_open())
							{
								file.read(data, size);
								file.close();
								PeoplezString newContent(data, size);
								size_t newHash = newContent.HashValue();
								if(newHash != hash)
								{
									content = newContent;
									if(compressed) content.Compress();
									hash = newHash;
								}
							}

							delete[] data;
						}
					}

					if(hashValue == hash) return Resource();
					else
					{
						PeoplezString uniqueContent = content;
						uniqueContent.ToUnique();
						return Resource(compressed, uniqueContent, RESOURCE_STATUS_UPDATED, hash, type);
					}
				}
				catch (...)
				{
					Logger::LogException("Error in ResourceHolderPreloaded::GetResource", __FILE__, __LINE__);
				}

				return Resource(false, "", RESOURCE_STATUS_ERROR, 0, FILE_TYPE::NONE);
			}
		} // namespace Resources
	} // namespace System
} // namespace Peoplez
