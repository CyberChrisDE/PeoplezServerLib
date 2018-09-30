/**
 * Copyright 2018 Christian Geldermann
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

#include "FastCGIClient.hpp"
#include "HttpFunctions.hpp"
#include "../../System/Logging/Logger.hpp"

#include <iostream>

extern "C"
{
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netdb.h>
#include<unistd.h>
#include<strings.h>
}

namespace Peoplez
{
	using namespace System::Logging;

	namespace Services
	{
		namespace Http
		{
			FastCGIClient::~FastCGIClient()
			{
			}

			uint8_t FastCGIClient::AddNVLengthInfo(PeoplezString &target, uint32_t len)
			{
				if(len < 128)
				{
					char const lenChar = (char)len;
					target.Append(&lenChar, 1);

					return 1;
				}
				else if(len <= 0xFFFF)
				{
					// Set first bit to 1 to indicate length > 127,
					// First 2 bytes zero because a single record can only have length of < 2^16
					uint8_t const lenStr[4] = {(uint8_t)0x80, (uint8_t)0x00, (uint8_t)((len >> 8) & 0xFF), (uint8_t)(len & 0xFF)};
					//{(128 | (len & 0xFF)), (len >> 7) & 0xFF, (len >> 15) & 0xFF, (len >> 23) & 0xFF};
					target.Append((char const*)lenStr, 4);

					return 4;
				}
				else return 0;
			}

			uint32_t FastCGIClient::AddNVPair(PeoplezString &target, PeoplezString const name, PeoplezString const value)
			{
				if(name.Length() > 0xFFFF || value.Length() > 0xFFFF)
				{
					std::cout << "!!!NAME/VALUE PAIR TOO LARGE!!!" << std::endl;
					return 0;
				}
				else
				{
					uint32_t const res = AddNVLengthInfo(target, name.Length())
						+ AddNVLengthInfo(target, value.Length())
						+ name.Length() + value.Length();

					target.Append(name);
					target.Append(value);

					return res;
				}
			}

			int FastCGIClient::Connect()
			{
				// Connect via Unix Domain Socket to PHP-FPM
				sockaddr_un servAddr;
				int const sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

				if(sockfd < 0) std::cout << "Error opening unix domain socket" << std::endl;

				servAddr.sun_family = AF_UNIX;
				pathToSocket.EnsureZeroTermination();
				strcpy(servAddr.sun_path, pathToSocket.GetData());

				if(connect(sockfd, (sockaddr *) &servAddr, sizeof(servAddr)) < 0)
				{
					close(sockfd);
					std::cout << "Error connecting to PHP interpreter" << std::endl;
					exit(1);
				}

				return sockfd;
			}

			char const * fcgi_type[11] = {"1 - BEGIN_REQUEST", "2 - ABORT_REQUEST", "3 - END_REQUEST", "4 - PARAMS", "5 - STDIN", "6 - STDOUT", "7 - STDERR", "8 - DATA", "9 - GET_VALUES", "10 - GET_VALUES_RESULT", "11 - UNKNOWN_TYPE"};

			char toHex(uint8_t in)
			{
				if(in >= 10) return in - 10 + 0x41;
				else return in + 0x30;
			}

			void printHex(uint8_t const * const str, size_t const strlen)
			{
				std::cout << "0x";

				for(size_t i = 0; i < strlen; ++i)
				{
					std::cout << toHex((str[i] >> 4) & 0x0F) << toHex(str[i] & 0x0F);
				}
			}

			uint32_t FastCGIClient::ReadNVPairLen(uint8_t const ** const str, uint8_t const * const end)
			{
				if(*str >= end) return 0;

				if((*str)[0] & 0x80)
				{
					// Long format
					if(*str - end < 4) return 0;

					uint32_t const res = (((uint32_t)(*str)[0] & 0x7F) << 24) | (((uint32_t)(*str)[1] & 0xFF) << 16)
							| (((uint32_t)(*str)[2]) << 8) | ((uint32_t)(*str)[3] & 0xFF);
					*str += 4;
					return res;
				}
				else
				{
					// Short format
					uint32_t const res = (*str)[0] & 0x7F;
					++(*str);
					return res;
				}
			}

			NameValuePair FastCGIClient::ReadNVPair(String::PeoplezString &msgBody)
			{
				uint8_t const * const str = (uint8_t const *) msgBody.GetData();
				uint8_t const * const end = str + msgBody.Length();
				uint8_t const * pos = str;

				// Determine name and value lengths
				uint32_t const nameLen = ReadNVPairLen(&pos, end);
				uint32_t const valueLen = ReadNVPairLen(&pos, end);
				unsigned const skipLen = pos - str;

				// Check valid length
				if(skipLen + nameLen + valueLen > msgBody.Length()) return NameValuePair();

				// Extract name and value
				NameValuePair const res = NameValuePair(msgBody.Substring(skipLen, nameLen), msgBody.Substring(skipLen + nameLen, valueLen));

				// Delete name-value pair from source
				msgBody = msgBody.Substring(skipLen + nameLen + valueLen);

				return res;
			}

			uint32_t FastCGIClient::DecodeMessage(String::PeoplezString &msgBuffer, PeoplezString &responseBuffer)
			{
				if(msgBuffer.Length() < 8) return 0;

				uint8_t const * const str = (uint8_t const *) msgBuffer.GetData();

				//uint8_t const version = str[0];
				uint8_t const type = str[1];
				uint16_t const requestId = (((unsigned)str[2]) << 8) + str[3];
				uint16_t const contentLen = (((unsigned)str[4]) << 8) + str[5];
				uint8_t const paddingLen = str[6];

				if(msgBuffer.Length() < contentLen + paddingLen + 8u) return 0;

				//std::cout << "Version:    " << (unsigned)version << std::endl;
				std::cout << "Type:       " << ((type > 11 || type < 1) ? "unknown": fcgi_type[type - 1]) << std::endl;
				std::cout << "ReqestId:   " << requestId << std::endl;
				std::cout << "ContentLen: " << contentLen << std::endl;
				std::cout << "PaddingLen: " << (unsigned)paddingLen << std::endl << std::endl;

				if(type == 6) responseBuffer += msgBuffer.Substring(8, contentLen);
				else if(type == 10)
				{
					PeoplezString msgBody = msgBuffer.Substring(8, contentLen);

					while(!msgBody.IsEmpty())
					{
						NameValuePair const nvPair = ReadNVPair(msgBody);

						if(nvPair.first.IsEmpty() || nvPair.second.IsEmpty())
						{
							Logger::LogException("Mal formatted FastCGI GET_VALUES_RESULT received", __FILE__, __LINE__);
							break;
						}

						try
						{
							if(nvPair.first.EqualTo("FCGI_MAX_CONNS", 14)) maxConns = nvPair.second.ToUInt64(10);
							else if(nvPair.first.EqualTo("FCGI_MAX_REQS", 13)) maxReqs = nvPair.second.ToUInt64(10);
							else if(nvPair.first.EqualTo("FCGI_MPXS_CONNS", 15)) mpxsConns = nvPair.second.EqualTo("1", 1) ? YES : NO;
						}
						catch (std::runtime_error & e)
						{
							Logger::LogException("Could not convert number in FCGI_GET_VALUES_RESULT message", __FILE__, __LINE__);
						}
					}
				}

				return contentLen + paddingLen + 8;
			}

			void FastCGIClient::GetApplicationInfo()
			{
				std::unique_lock<std::mutex> const lock(mut);

				char const * const getValuesMsg = "\x01\x09\x00\x00\x00\x30\x00\x00\x0E\0FCGI_MAX_CONNS\x0D\0FCGI_MAX_REQS\x0F\0FCGI_MPXS_CONNS";

				// Connect to FCGI application
				int const sockfd = Connect();

				// Send message
				send(sockfd, getValuesMsg, 56, 0);

				int const BUFF_SIZE = 10000;
				char buffer[BUFF_SIZE];
				PeoplezString readBuffer;
				PeoplezString responseBuffer;

				while(true)
				{
					ssize_t const n = recv(sockfd, buffer, BUFF_SIZE, 0);

					std::cout << "Reiceived " << n << " bytes" << std::endl;

					if(n < 0)
					{
						std::cout << "Receiving error" << std::endl;
						exit(1);
					}
					else if(n == 0) break;
					else
					{
						readBuffer.Append(buffer, n);

						if(readBuffer.Length() >= 8 && DecodeMessage(readBuffer, responseBuffer)) break;
					}
				}

				//std::cout << "Max. Connections: " << maxConns << std::endl;
				//std::cout << "Max. Requests:    " << maxReqs << std::endl;
				//std::cout << "MPXS Connections: " << mpxsConns << std::endl;

				close(sockfd);
			}

			void FastCGIClient::ProcessRequest(HttpContext &context)
			{
				// Check whether requested HTTP method is allowed
				if(config.allowedRequests.Get(context.request.HttpMethod()))
				{
					context.response.SetOther(HttpStatusCode::METHOD_NOT_ALLOWED);
					// TODO: Add "Allow" header field
					return;
				}

				size_t const rawUrlSize = context.request.rawUrl.size();
				PeoplezString fileName = rawUrlSize ? context.request.rawUrl[rawUrlSize - 1] : PeoplezString();
				if(fileName.Length() > 4) fileName = fileName.Substring(fileName.Length() - 4);
				fileName.ToLower_ASCII();

				if(!fileName.EqualTo(".php", 4))
				{
					context.response.SetOther(HttpStatusCode::NOT_FOUND);
					return;
				}

				std::unique_lock<std::mutex> const lock(mut);

				PeoplezString FCGIRequest("\x01\x01\x00\x01\x00\x08\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00"
						// Header of FCGI_PARAMS message
						"\x01\x04\x00\x01\x00\x00\x00\x00"
						// NV-Pair GATEWAY_INTERFACE
						"\x11\x07GATEWAY_INTERFACECGI/1.1"
						// NV-Pair SERVER_SOFTWARE
						"\x0F\x11SERVER_SOFTWAREPeoplezServer/0.1", 84);
				uint32_t contentLength = 60;
				//contentLength += AddNVPair(FCGIRequest, PeoplezString("GATEWAY_INTERFACE", 17), PeoplezString("CGI/1.1", 7));
				//contentLength += AddNVPair(FCGIRequest, PeoplezString("SERVER_SOFTWARE", 15), PeoplezString("PeoplezServer/0.1", 17));
				contentLength += AddNVPair(FCGIRequest, PeoplezString("REQUEST_METHOD", 14), HttpFunctions::ToPString(context.request.HttpMethod()));
				//contentLength += AddNVPair(FCGIHeader, PeoplezString("CONTENT_TYPE", 12), PeoplezString());
				//contentLength += AddNVPair(FCGIHeader, PeoplezString("CONTENT_LENGTH", 14), PeoplezString());

				PeoplezString path = context.request.RequestUri();

				// Ensure that path begins with a '/'
				if(path.IsEmpty() || path[0] != '/')
				{
					PeoplezString newPath(path.Length() + 1);
					newPath.Append("/", 1);
					newPath += path;
					path = newPath;
				}

				{	// Remove trailing '/'s
					size_t len = path.Length();
					while(len - 1 && path[len - 1] == '/') --len;
					if(len < path.Length()) path = path.Substring(0, len);
				}

				contentLength += AddNVPair(FCGIRequest, PeoplezString("SCRIPT_NAME", 11), path);
				if(!context.request.QueryString().IsEmpty())
					contentLength += AddNVPair(FCGIRequest, PeoplezString("QUERY_STRING", 12), context.request.QueryString());
				contentLength += AddNVPair(FCGIRequest, PeoplezString("REMOTE_ADDR", 11), PeoplezString("127.0.0.1", 9)); // IP of requesting client
				contentLength += AddNVPair(FCGIRequest, PeoplezString("SERVER_NAME", 11), PeoplezString(config.serverName));
				contentLength += AddNVPair(FCGIRequest, PeoplezString("SERVER_PORT", 11), PeoplezString("8080", 4));
				contentLength += AddNVPair(FCGIRequest, PeoplezString("SERVER_PROTOCOL", 15), PeoplezString("HTTP/1.1", 8));
				contentLength += AddNVPair(FCGIRequest, PeoplezString("HTTP_HOST", 9), context.request.Host());
				contentLength += AddNVPair(FCGIRequest, PeoplezString("DOCUMENT_ROOT", 13), PeoplezString(config.documentRoot));
				contentLength += AddNVPair(FCGIRequest, PeoplezString("SCRIPT_FILENAME", 15), PeoplezString(config.documentRoot) + path);

				assert(contentLength <= 0xFFFF);

				FCGIRequest[21] = (char)(contentLength & 0xFF);
				FCGIRequest[20] = (char)(contentLength >> 8);

				FCGIRequest.Append("\x01\x04\x00\x01\x00\x00\x00\x00", 8);

				// Connect to FCGI application
				int const sockfd = Connect();

				// Send the 3 request messages
				send(sockfd, FCGIRequest.GetData(), FCGIRequest.Length(), 0);

				int const BUFF_SIZE = 10000;
				char buffer[BUFF_SIZE];
				PeoplezString readBuffer;
				PeoplezString responseBuffer;

				while(true)
				{
					ssize_t const n = recv(sockfd, buffer, BUFF_SIZE, 0);

					std::cout << "Reiceived " << n << " bytes" << std::endl;

					if(n < 0)
					{
						std::cout << "Receiving error" << std::endl;
						exit(1);
					}
					else if(n == 0) break;
					else
					{
						readBuffer.Append(buffer, n);

						while(readBuffer.Length() >= 8)
						{
							uint32_t const msgLen = DecodeMessage(readBuffer, responseBuffer);
							if(msgLen) readBuffer = readBuffer.Substring(msgLen);
							else break;
						}
					}
				}

				close(sockfd);

				size_t const dnlPos = responseBuffer.FindDoubleNewLine(0);

				if(dnlPos >= responseBuffer.Length())
				{
					std::cout << "New line not found" << std::endl;
					return;
				}

				PeoplezString headers = responseBuffer.Substring(0, dnlPos);
				PeoplezString body = responseBuffer.Substring(dnlPos + 4);

				HttpStatusCode statusCode = HttpStatusCode::OK;
				PeoplezString contentType;

				// Decode header
				std::vector<NameValuePair> headLines;
				headers.SplitToPairs(headLines, '\n', ':', true);

				for(size_t i = 0; i < headLines.size(); ++i)
				{
					switch(headLines[i].first.Length())
					{
					case 6:
						if(headLines[i].first.EqualTo("Status", 6))
						{
							size_t const spacePos1 = headLines[i].second.Find(' ');
							size_t const spacePos2 = headLines[i].second.Find(' ', spacePos1 + 1);

							if(spacePos1 == PeoplezString::NPOS)
							{
								statusCode = HttpStatusCode::BAD_GATEWAY;
								break;
							}
							else
							{
								// Extract status code number
								PeoplezString const codeString =
										(spacePos2 == PeoplezString::NPOS) ?
												headLines[i].second.Substring(spacePos1 + 1) :
												headLines[i].second.Substring(spacePos1 + 1, spacePos2 - spacePos1 - 1);

								// Try parsing the number to unsigned int
								try
								{
									statusCode = (HttpStatusCode)codeString.ToInt<unsigned int>(10);
								}
								catch(std::runtime_error const &err)
								{
									// Parsing failed
									statusCode = HttpStatusCode::BAD_GATEWAY;
									break;
								}

								// Check whether parsed status code is a valid HTTP Status Code
								if(!isValidStatusCode(statusCode)) statusCode = HttpStatusCode::BAD_GATEWAY;
							}
						}
						else if(headLines[i].first.EqualTo("Content-type", 12))
						{
							contentType = headLines[i].second;
						}
						break;
					}
				}

				// Generate HTTP response
				contentType.IsEmpty() ?
						context.response.SetWithBody(statusCode, body) :
						context.response.SetWithBody(statusCode, contentType, body);
			}
		} /* namespace Http */
	} /* namespace Services */
} /* namespace Peoplez */
