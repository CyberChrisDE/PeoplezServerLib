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
#include "HttpFunctions.hpp"

// Local includes
#include "../../System/Logging/Logger.hpp"

// Extern includes
#include <fstream>
#include <zlib.h>
extern "C"
{
#include <sys/stat.h>
}

#ifdef __unix__
	extern "C"
	{
	#include <unistd.h>
	}
	#define EXIST F_OK
	#define EXEC X_OK
	#define WRITE W_OK
	#define READ R_OK
#else
	extern "C"
	{
	#include <io.h>
	}
	/**
	 * @def EXIST
	 * @brief Defines flag to test for existence of a file
	 */
	#define EXIST 00
	/**
	 * @def EXEC
	 * @brief Defines flag to test for execute/search permission
	 */
	#define EXEC 01
	/**
	 * @def WRITE
	 * @brief Defines flag to test for write permission
	 */
	#define WRITE 02
	/**
	 * @def READ
	 * @brief Defines flag to test for read permission
	 */
	#define READ 04
#endif

/**
 * @def SHORTEN
 * @brief If defined, shortening file content is enabled
 */
#define SHORTEN

// External namespaces
using namespace std;

namespace Peoplez
{
	// Local namespaces
	using namespace String;
	using namespace System::Logging;

	namespace Services
	{
		namespace Http
		{
			Language HttpFunctions::IdentifyLanguage(char const * const lang) noexcept
			{
				//Kopieren des Codes und dabei ToLower
				char code[2];
				code[0] = lang[0] >= 97 ? lang[0] : lang[0] + 32;
				code[1] = lang[1] >= 97 ? lang[0] : lang[0] + 32;

				if(code[0] == 'd' && code[1] == 'e') return LANGUAGE::DE;
				else if(code[0] == 'e' && code[1] == 'n') return LANGUAGE::EN;
				else if(code[0] == 'f' && code[1] == 'r') return LANGUAGE::FR;
				else return (Language) -1;
			}

	 		int HttpFunctions::ReadBinaryFile(PeoplezString path, char * * const data)
			{
				try
				{
					path.EnsureZeroTermination();
					struct stat status;

					if(lstat(path.GetData(), &status) != 0) Logger::LogException((PeoplezString)"No such file: " + path, __FILE__, __LINE__);
					else
					{
						*data = (char *)malloc(status.st_size);//new char[status.st_size];
						ifstream file(path.GetData(), ios_base::in | ios_base::binary);
						if(!file) Logger::LogEvent((PeoplezString)"File could not be opened: " + path);
						else
						{
							file.read(*data, status.st_size);
							file.close();
						}

						return status.st_size;
					}
				}
				catch(...)
				{
					Logger::LogException((PeoplezString)"Error in Core::ReadBinaryFile with file " + path, __FILE__, __LINE__);
				}

				return 0;
			}

			PeoplezString HttpFunctions::ReadFile(PeoplezString path, FileBeginsWith const beginsWith)
			{
				try
				{
					path.EnsureZeroTermination();

					if(access(path.GetData(), EXIST) != -1 && access(path.GetData(), READ) != -1)
					{
						char * buffer = 0;
						std::fstream file(path.GetData(), std::ios::in);
						//Länge der Datei ermitteln
						file.seekg(0, ios::end);
						const int length = file.tellg();
						file.seekg(0, ios::beg);
						//Lesen der Datei
						buffer = new char[length];
						file.read(buffer, length);
						file.close();
						const PeoplezString result(buffer, length);
						delete[] buffer;

					#ifdef SHORTEN
						return Shorten(result, beginsWith);
					#else
						return result;
					#endif
					}
					else Logger::LogException((PeoplezString)"Could not read from File: " + path + "; Reason: " + String::PeoplezString((char const *) strerror(errno)), __FILE__, __LINE__);
				}
				catch (...)
				{
					Logger::LogException((PeoplezString)"Exception in Core::ReadFile with file " + path, __FILE__, __LINE__);
				}
				throw "File could not be opened";
			}

			String::PeoplezString HttpFunctions::FileTypeToContentType(FileType const type)
			{
				try
				{
					switch(FileCategoryOf(type))
					{
					case FILE_TYPE::IMAGE:
						switch(type)
						{
						case FILE_TYPE::IMAGE_GIF:
							return PeoplezString("image/gif", 9);
							break;
						case FILE_TYPE::IMAGE_JPEG:
							return PeoplezString("image/jpeg", 10);
							break;
						case FILE_TYPE::IMAGE_PNG:
							return PeoplezString("image/png", 9);
							break;
						case FILE_TYPE::IMAGE_TIFF:
							return PeoplezString("image/tiff", 10);
							break;
						case FILE_TYPE::IMAGE_SVG:
							return PeoplezString("image/svg+xml", 13);
							break;
						default:
							return PeoplezString();
							break;
						}
						break;
					case FILE_TYPE::TEXT:
						switch(type)
						{
						case FILE_TYPE::TEXT_CSS:
							return PeoplezString("text/css", 8);
							break;
						case FILE_TYPE::TEXT_HTML:
							return PeoplezString("text/html", 9);
							break;
						case FILE_TYPE::TEXT_SCRIPT_JS:
							return PeoplezString("text/javascript", 15);
							break;
						default:
							return PeoplezString();
							break;
						}
						break;
					default:
						return PeoplezString();
						break;
					}
				}
				catch (...)
				{
					Logger::LogException("Error in HttpClientInfo::FileTypeToContentType", __FILE__, __LINE__);
				}

				return PeoplezString();
			}

			HttpMethods HttpFunctions::ToHttpMethod(PeoplezString const & str)
			{
				//INFO:
				//Http methods are case sensitive. So we do not need a ToUpper

				//Suchen
				switch(str.Length())
				{
				case 3:
					if(str.EqualTo("GET", 3)) return HttpMethods::GET;
					else if(str.EqualTo("PUT", 3)) return HttpMethods::PUT;
				break;
				case 4:
					if(str.EqualTo("POST", 4)) return HttpMethods::POST;
					else if(str.EqualTo("HEAD", 4)) return HttpMethods::HEAD;
					break;
				case 5:
					if(str.EqualTo("PATCH", 5)) return HttpMethods::PATCH;
					break;
				case 6:
					if(str.EqualTo("DELETE", 6)) return HttpMethods::DELETE;
					else if(str.EqualTo("TRACE", 6)) return HttpMethods::TRACE;
					break;
				case 7:
					if(str.EqualTo("OPTIONS", 7)) return HttpMethods::OPTIONS;
					else if(str.EqualTo("CONNECT", 7)) return HttpMethods::CONNECT;
					break;
				default:
					break;
				}

				return HttpMethods::UNKNOWN;
			}

			PeoplezString HttpFunctions::ToPString(HttpMethods const method)
			{
				switch (method) {
					case HttpMethods::OPTIONS:
						return "OPTIONS";
					case HttpMethods::GET:
						return "GET";
					case HttpMethods::HEAD:
						return "HEAD";
					case HttpMethods::POST:
						return "POST";
					case HttpMethods::PUT:
						return "PUT";
					case HttpMethods::PATCH:
						return "PATCH";
					case HttpMethods::DELETE:
						return "DELETE";
					case HttpMethods::TRACE:
						return "TRACE";
					case HttpMethods::CONNECT:
						return "CONNECT";
					default:
						return "";
				}
			}

	 		PeoplezString HttpFunctions::Shorten(PeoplezString text, FileBeginsWith const beginsWith)
	 		{
	 			char *const data = new char[text.Length() + 1];
	 			char *dataNew = 0;
	 			char *temp = data;
	 			int erased = 0;
	 			char *const end = data + text.Length();

	 			//TODO durch memmove ersetzen
	 			std::copy(text.GetData(), text.GetData() + text.Length(), data);
	 			data[text.Length()] = '\0';

	 			if(beginsWith == HTML) erased = ShortenHtml(&temp, end);
	 			else if(beginsWith == CSS) erased = ShortenCss(&temp, end);
	 			else if(beginsWith == SCRIPT) erased = ShortenScript(&temp, end);

	 			if(erased > 0)
	 			{
	 				dataNew = new char[text.Length() + 1 - erased];
	 				char *posNew = dataNew;

	 				for(char *posOld = data; posOld < end; ++posOld)
	 				{
	 					if(*posOld != '\0')
	 					{
	 						*posNew = *posOld;
	 						++posNew;
	 					}
	 				}

	 				PeoplezString result(dataNew, text.Length() - erased);

	 				delete[] data;
	 				delete[] dataNew;

	 				return result;
	 			}
	 			else
	 			{
	 				PeoplezString result(data, text.Length() - erased);

	 				delete[] data;

	 				return result;
	 			}
	 		}

	 		int HttpFunctions::ShortenHtml(char * * const text, char * const end)
	 		{
	 			char *pos = *text;
	 			int erased = 0;

	 			bool spaceBefore = false;

	 			for(; pos < end; ++pos)
	 			{
	 				if(*pos == ' ' || *pos == '\t' || *pos == '\r' || *pos == '\n')
	 				{
	 					if(spaceBefore)
	 					{
	 						*pos = '\0';
	 						++erased;
	 					}
	 					else spaceBefore = true;
	 				}
	 				else if(*pos == '<')
	 				{
	 					if((end - pos) > 5 && *(pos + 1) == 's' && *(pos + 2) == 't' && *(pos + 3) == 'y' && *(pos + 4) == 'l' && *(pos + 5) == 'e')
	 					{
	 						erased += ShortenHtmlTag(&pos, end);
	 						if(pos < end) ++pos;
	 						if(pos < end) erased += ShortenCss(&pos, end);
	 					}
	 					else if((end - pos) > 6 && *(pos + 1) == 's' && *(pos + 2) == 'c' && *(pos + 3) == 'r' && *(pos + 4) == 'i' && *(pos + 5) == 'p' && *(pos + 6) == 't')
	 					{
	 						erased += ShortenHtmlTag(&pos, end);
	 						if(pos < end) ++pos;
	 						if(pos < end) erased += ShortenScript(&pos, end);
	 					}
	 					else erased += ShortenHtmlTag(&pos, end);

	 					spaceBefore = false;
	 				}
	 				else spaceBefore = false;
	 			}

	 			*text = pos;

	 			return erased;
	 		}

	 		int HttpFunctions::ShortenHtmlTag(char * * const text, char * const end)
	 		{
	 			char *pos = *text;
	 			int erased = 0;

	 			for(; pos < end; ++pos)
	 			{
	 				if(*pos == '>')
	 				{
	 					break;
	 				}
	 				else if(*pos == '\"')
	 				{
	 					erased += ShortenText(&pos, end);
	 				}
	 				else if((*pos == ' ' || *pos == '\t') && pos < end - 1 && (*(pos + 1) == ' ' || *pos == '\t'))
	 				{
	 					*pos = '\0';
	 					++erased;
	 				}
	 				else if(*pos == '\r' || *pos == '\n')
	 				{
	 					*pos = ' ';
	 					pos -= 2;
	 				}
	 			}

	 			*text = pos;

	 			return erased;
	 		}

	 		int HttpFunctions::ShortenText(char * * const text, char * const end)
	 		{
	 			char *pos = *text + 1;

	 			for(; pos < end && *pos != '\"'; ++pos);

	 			*text = pos;

	 			return 0;
	 		}

	 		int HttpFunctions::ShortenCss(char * * const text, char * const end)
	 		{
	 			char *pos = *text;
	 			int erased = 0;

	 			for(; pos < end; ++pos)
	 			{
	 				if((*pos == ' ' || *pos == '\t') && pos < end - 1 && (*(pos + 1) == ' ' || *pos == '\t'))
	 				{
	 					*pos = '\0';
	 					++erased;
	 				}
	 				else if(*pos == '\r' || *pos == '\n')
	 				{
	 					*pos = ' ';
	 					pos -= 2;
	 				}
	 				else if(*pos == '{')
	 				{
	 					erased += ShortenCssArea(&pos, end);
	 				}
	 				else if(pos < end - 6 && *pos == '<' && *(pos + 1) == '/' && *(pos + 2) == 's' && *(pos + 3) == 't' && *(pos + 4) == 'y' && *(pos + 5) == 'l' && *(pos + 6) == 'e')
	 				{
	 					erased += ShortenHtml(&pos, end);
	 				}
	 				else if(*pos == '/' && end - pos > 1 && *(pos + 1) == '*')
	 				{
	 					erased += ShortenCommentMultiLine(&pos, end);
	 				}
	 			}

	 			*text = pos;

	 			return erased;
	 		}

	 		int HttpFunctions::ShortenCssArea(char * * const text, char * const end)
	 		{
	 			char *pos = *text;
	 			int erased = 0;

	 			for(; pos < end; ++pos)
	 			{
	 				if(*pos == ' ' || *pos == '\t' || *pos == '\r' || *pos == '\n')
	 				{
	 					*pos = '\0';
	 					++erased;
	 				}
	 				else if(*pos == '}') break;
	 				else if(*pos == '/' && end - pos > 1 && *(pos + 1) == '*')
	 				{
	 					erased += ShortenCommentMultiLine(&pos, end);
	 				}
	 				else
	 				{
	 					erased += ShortenCssTag(&pos, end);
	 				}
	 			}

	 			*text = pos;

	 			return erased;
	 		}

	 		int HttpFunctions::ShortenCssTag(char * * const text, char * const end)
	 		{
	 			char *pos = *text;
	 			int erased = 0;

	 			for(; pos < end; ++pos)
	 			{
	 				if((*pos == ' ' || *pos == '\t') && pos < end - 1 && (*(pos + 1) == ' ' || *(pos + 1) == '\t' || *(pos + 1) == '{' || *(pos + 1) == '}' || *(pos + 1) == '(' || *(pos + 1) == ')'))
	 				{
	 					*pos = '\0';
	 					++erased;
	 				}
	 				else if(end - pos > 1 && (*pos == '{' || *pos == '}' || *pos == '(' || *pos == ')' || *pos ==':') && (*(pos + 1) == ' ' || *(pos + 1) == '\t'))
	 				{
	 					*(pos + 1) = '\0';
	 					++erased;
	 				}
	 				else if(*pos == '\r' || *pos == '\n')
	 				{
	 					*pos = ' ';
	 					pos -= 2;
	 				}
	 				else if(*pos == ';') break;
	 				else if(*pos == '\"') erased += ShortenText(&pos, end);
	 				else if(*pos == '/' && end - pos > 1 && *(pos + 1) == '*')
	 				{
	 					erased += ShortenCommentMultiLine(&pos, end);
	 				}
	 			}

	 			*text = pos;

	 			return erased;
	 		}

	 		int HttpFunctions::ShortenScript(char * * const text, char * const end)
	 		{
	 			char *pos = *text;
	 			int erased = 0;

	 			for(; pos < end; ++pos)
	 			{
	 				if(*pos == ' ' || *pos == '\t' || *pos == '\r' || *pos == '\n')
	 				{
	 					*pos = '\0';
	 					++erased;
	 				}
	 				else if(end - pos >= 8 && *pos == 'f' && *(pos + 1) == 'u' && *(pos + 2) == 'n' && *(pos + 3) == 'c' && *(pos + 4) == 't' && *(pos + 5) == 'i' && *(pos + 6) == 'o' && *(pos + 7) == 'n')
	 				{
	 					erased += ShortenScriptFunction(&pos, end);
	 				}
	 				else if(end - pos >= 8 && *pos == '<' && *(pos + 1) == '/' && *(pos + 2) == 's' && *(pos + 3) == 'c' && *(pos + 4) == 'r' && *(pos + 5) == 'i' && *(pos + 6) == 'p' && *(pos + 7) == 't')
	 				{
	 					erased += ShortenHtml(&pos, end);
	 				}
	 				else if(*pos == '/' && end - pos > 1 && *(pos + 1) == '/')
	 				{
	 					erased += ShortenCommentSingleLine(&pos, end);
	 				}
	 				else
	 				{
	 					erased += ShortenScriptTag(&pos, end);
	 				}
	 			}

	 			*text = pos;

	 			return erased;
	 		}

	 		int HttpFunctions::ShortenScriptFunction(char * * const text, char * const end)
	 		{
	 			char *pos = *text;
	 			int erased = 0;

	 			for(; pos < end; ++pos)
	 			{
	 				if((*pos == ' ' || *pos == '\t') && end - pos >= 2 && (*(pos + 1) == '+' || *(pos + 1) == '-' || *(pos + 1) == '=' || *(pos + 1) == '/' || *(pos + 1) == '{' || *(pos + 1) == '}' || *(pos + 1) == '(' || *(pos + 1) == ')' || *(pos + 1) == ';' ||  *(pos + 1) == ' ' || *(pos + 1) == '\t'))
	 				{
	 					*pos = '\0';
	 					++erased;
	 				}
	 				else if(end - pos >= 2 && (*pos == '+' || *pos == '-' || *pos == '=' || *pos == '/' || *pos == '{' || *pos == '}' || *pos == '(' || *pos == ')' || *pos == ';') && (*(pos + 1) == ' ' || *(pos + 1) == '\t'))
	 				{
	 					*(pos + 1) = '\0';
	 					++erased;
	 				}
	 				else if(*pos == '\r' || *pos == '\n')
	 				{
	 					*pos = ' ';
	 					pos -= 2;
	 				}
	 				else if(*pos == '{')
	 				{
	 					erased += ShortenScriptFunction(&pos, end);
	 				}
	 				else if(*pos == '}' || pos != *text) break;
	 				else if(*pos == '/' && end - pos > 1 && *(pos + 1) == '*')
	 				{
	 					erased += ShortenCommentMultiLine(&pos, end);
	 				}
	 				else if(*pos == '/' && end - pos > 1 && *(pos + 1) == '/')
	 				{
	 					erased += ShortenCommentSingleLine(&pos, end);
	 				}
	 			}

	 			*text = pos;

	 			return erased;
	 		}

	 		int HttpFunctions::ShortenScriptTag(char * * const text, char * const end)
	 		{
	 			char *pos = *text;
	 			int erased = 0;

	 			for(; pos < end && *pos != ';'; ++pos)
	 			{
	 				if((*pos == ' ' || *pos == '\t') && end - pos >= 2 && (*(pos + 1) == '+' || *(pos + 1) == '-' || *(pos + 1) == '=' || *(pos + 1) == '/' || *(pos + 1) == '{' || *(pos + 1) == '}' || *(pos + 1) == '(' || *(pos + 1) == ')' || *(pos + 1) == ' ' || *(pos + 1) == '\t'))
	 				{
	 					*pos = '\0';
	 					++erased;
	 				}
	 				else if(end - pos >= 2 && (*pos == '+' || *pos == '-' || *pos == '=' || *pos == '/' || *pos == '{' || *pos == '}' || *pos == '(' || *pos == ')') && (*(pos + 1) == ' ' || *(pos + 1) == '\t'))
	 				{
	 					*(pos + 1) = '\0';
	 					++erased;
	 				}
	 				else if(*pos == '\r' || *pos == '\n')
	 				{
	 					*pos = ' ';
	 					pos -= 2;
	 				}
	 				else if(*pos == '/' && end - pos > 1 && *(pos + 1) == '*')
	 				{
	 					erased += ShortenCommentMultiLine(&pos, end);
	 				}
	 				else if(*pos == '/' && end - pos > 1 && *(pos + 1) == '/')
	 				{
	 					erased += ShortenCommentSingleLine(&pos, end);
	 				}
	 			}

	 			*text = pos;

	 			return erased;
	 		}

	 		int HttpFunctions::ShortenCommentMultiLine(char * * const text, char * const end)
	 		{
	 			int erased = 0;

	 			for(char *pos = *text; pos < end; ++pos)
	 			{
	 				if(*pos == '*' && end - pos > 1 && *(pos + 1) == '/')
	 				{
	 					*pos = '\0';
	 					*(pos + 1) = '\0';
	 					erased += 2;
	 					break;
	 				}
	 				else
	 				{
	 					*pos = '\0';
	 					++erased;
	 				}
	 			}

	 			return erased;
	 		}

	 		int HttpFunctions::ShortenCommentSingleLine(char * * const text, char * const end)
	 		{
	 			char *pos = *text;
	 			int erased = 0;

	 			for(; pos < end && *pos != '\r' && *pos != '\n'; ++pos)
	 			{
	 				*pos = '\0';
	 				++erased;
	 			}

	 			*text = pos;

	 			return erased;
	 		}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
