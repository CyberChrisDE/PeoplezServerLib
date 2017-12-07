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
#include "HttpContext.hpp"

// Local includes
#include "../../System/Logging/Logger.hpp"
#include "../../General/MimeOperations.hpp"
#include "HttpFunctions.hpp"

/**
 * @def MIN_FIRST_LINE_LENGTH
 * @brief Minimum length of the first http header line
 * @details Minimum length in bytes that the first line of the http header must have so that the request can be a valid http request
 * A minimum request could be: "GET / HTTP/1.0"
 */
#define MIN_FIRST_LINE_LENGTH 14

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
			const String::PeoplezString HttpContext::COMPARE_STRING_ACCEPT_LANGUAGE = "Accept-Language"; //15
			const String::PeoplezString HttpContext::COMPARE_STRING_CONNECTION = "Connection"; //10
			const String::PeoplezString HttpContext::COMPARE_STRING_IF_NONE_MATCH = "If-None-Match"; //13
			const String::PeoplezString HttpContext::COMPARE_STRING_CONTENT_LENGTH = "Content-Length"; //14
			const String::PeoplezString HttpContext::COMPARE_STRING_COOKIE = "Cookie"; //6
			//thread_local std::vector<String::PeoplezString> HttpContext::threadLocalVector;

			HttpStatusCode HttpContext::HandleHeader() noexcept
			{
				try
				{
					size_t positionStart = 0;
					size_t positionEnd = InputBuffer.FindEndOfLine(positionStart);

					if(positionEnd < MIN_FIRST_LINE_LENGTH || positionEnd == PeoplezString::NPOS) return HttpStatusCode::BAD_REQUEST; //First line invalid
					else
					{
						PeoplezString firstLine(InputBuffer.Substring(0, positionEnd));
						//Zerlegen der ersten Zeile ("Request-Line")
						//std::vector<PeoplezString> &lineElements = threadLocalVector;
						std::vector<PeoplezString> lineElements;
						firstLine.Split<true>(lineElements, ' ');
						//firstLine.Split(threadLocalVector, ' ', true);

						if(lineElements.size() < 3) return HttpStatusCode::BAD_REQUEST;
						else
						{
							request.httpMethod = HttpFunctions::ToHttpMethod(lineElements[0]);
							lineElements[1].Split<true>(request.rawUrl, '/');

							//Unescaping of entries
							for(register uint64_t i = request.rawUrl.size(); i > 0; --i) request.rawUrl[i - 1].DecodeUrl();

							positionStart = positionEnd + 2;
							positionEnd = InputBuffer.FindEndOfLine(positionStart);
							size_t seperateCharPos = 0;

							while(positionEnd != PeoplezString::NPOS && positionEnd > positionStart)
							{
								seperateCharPos = InputBuffer.Find(':', positionStart);

								if(positionStart < seperateCharPos && seperateCharPos < positionEnd)
								{
									InsertHeader(InputBuffer.Substring(positionStart, seperateCharPos - positionStart), InputBuffer.Substring(seperateCharPos + 1, positionEnd - seperateCharPos - 1));
								}
								else return HttpStatusCode::BAD_REQUEST;

								positionStart = positionEnd + 2;
								positionEnd = InputBuffer.FindEndOfLine(positionStart);
							}
						}
					}

					return HttpStatusCode::OK;
				}
				catch (...)
				{
					Logger::LogException("Error handling HTTP-Header", __FILE__, __LINE__);
				}

				//threadLocalVector.clear();

				return HttpStatusCode::INTERNAL_SERVER_ERROR;
			}

//			HttpStatusCode HttpContext::HandleRequestStream(RequestStream & readInfo) noexcept
//			{
//				try
//				{
//					switch(readInfo.status)
//					{
//					case READ_STATUS_COMPLETE:
//					{
//						size_t positionEnd = readInfo.input.FindEndOfLine();
//
//						if(positionEnd == PeoplezString::NPOS) return HttpStatusCode::OK;
//						else if(positionEnd < MIN_FIRST_LINE_LENGTH) return HttpStatusCode::BAD_REQUEST;
//					}
//						readInfo.status = READ_STATUS_HEADERS;
//					case READ_STATUS_HEADERS:
//
//					case READ_STATUS_BODY:
//						break;
//					}
//				}
//				catch(...)
//				{
//					Logger::LogException("Error handling http request stream", __FILE__, __LINE__);
//				}
//
//				return HttpStatusCode::INTERNAL_SERVER_ERROR;
//			}

			HttpStatusCode HttpContext::HandleBody(size_t const size) noexcept
			{
				try
				{
					if(request.HttpMethod() == HttpMethods::POST) //Auslesen des Bodys
					{
						PeoplezString body = InputBuffer.Substring(0, size);

						if(request.contentType == MimeType::APPLICATION_X_WWW_FORM_URLENCODED) body.SplitToPairs<PostParam>(request.postParams, &PostParam::Name, &PostParam::Value, '&', '=', true);
						else if(request.contentType == MimeType::MULTIPART_FORM_DATA) return HandleMultipartFormData(body, request.postParams, request.boundary);
						else Logger::LogEvent("Unknown mime type");
//						else if(request.contentType == MimeType::MULTIPART_FORM_DATA)
//						{	// multipart/form-data
//
//							size_t pos = 0;
//							request.boundary = (PeoplezString)"--" + request.boundary;
//
//							if(body.Compare(pos + 2, request.boundary.Length(), request.boundary))
//							{
//								pos += 2 + request.boundary.Length();
//
//								while(body.Compare(pos, "\r\n", 2))
//								{
//									PeoplezString name;
//									pos += 2;
//
//									if(!body.Compare(pos, "Content-Disposition: ", 21)) return BAD_REQUEST;
//									pos += 21;
//
//									if(body.Compare(pos, "form-data; ", 11))
//									{
//										bool nameFound = false;
//										size_t contentEnd = 0;
//										pos += 11;
//
//										if(body.Compare(pos, "name=\"", 6))
//										{
//											pos += 6;
//
//											size_t const quotePos = body.Find('\"', pos);
//											name = body.Substring(pos, quotePos - pos);
//											if(quotePos == PeoplezString::NPOS) return BAD_REQUEST;
//											pos = quotePos + 1;
//
//											nameFound = true;
//										}
//
//										if(body.Compare(pos, "filename=\"", 10))
//										{
//											pos += 10;
//
//											if(nameFound)
//											{
//												pos = body.Find('\"', pos);
//												if(pos == PeoplezString::NPOS) return BAD_REQUEST;
//												++pos;
//											}
//											else
//											{
//												size_t const quotePos = body.Find('\"', pos);
//												name = body.Substring(pos, quotePos - pos);
//												if(quotePos == PeoplezString::NPOS) return BAD_REQUEST;
//												pos = quotePos + 1;
//											}
//										}
//										else if(!nameFound) return BAD_REQUEST;
//
//										if(!body.Compare(pos, "\r\n", 2)) return BAD_REQUEST;
//										pos += 2;
//
////										if(body.Compare(pos, "ContentType: ", 13))
////										{
////											pos = body.Find('\r', pos) + 1;
////											if(!body.Compare(pos++, "\n", 1)) return BAD_REQUEST;
////										}
//
//										if(!body.Compare(pos, "\r\n", 2)) return BAD_REQUEST;
//										pos += 2;
//										contentEnd = pos;
//
//										while((contentEnd = body.FindEndOfLine(contentEnd)) != PeoplezString::NPOS && !body.Compare(contentEnd + 2, request.boundary.Length(), request.boundary))
//										{
//											contentEnd += 2;
//										}
//
//										if(contentEnd == PeoplezString::NPOS) return BAD_REQUEST;
//										request.postParams.push_back(NameValuePair(name, body.Substring(pos, contentEnd - pos)));
//										pos = contentEnd + 2 + request.boundary.Length();
//									}
//									else if(body.Compare(pos, "file; ", 6))
//									{
//										pos += 6;
//										//TODOPeoplez Implementieren
//									}
//									else return BAD_REQUEST;
//								}
//							}
//							else return BAD_REQUEST;
//						}
					}

					return HttpStatusCode::OK;
				}
				catch (...)
				{
					Logger::LogException("Error handling HTTP-Body", __FILE__, __LINE__);
				}
				return HttpStatusCode::INTERNAL_SERVER_ERROR;
			}

			HttpStatusCode HttpContext::HandleMultipartFormData(PeoplezString const src, std::vector<PostParam> &dest, PeoplezString boundary) noexcept
			{
				try
				{
					//TODO Somthing is wrong with variables "name" and "content"!
					PeoplezString name, content;
					size_t posStart = 0, posEnd = 0, contentStart = 0;
					size_t const srcLen = src.Length(), boundaryLen = boundary.Length() + 2;
					unsigned int phase = 0;

					boundary = (PeoplezString)"--" + boundary;

					while(phase < 6 && posEnd < srcLen && posEnd != PeoplezString::NPOS)
					{
						PostParam param;
						posStart = posEnd + 2;
						posEnd = src.FindEndOfLine(posStart);

						switch(phase)
						{
						case 0:
							//Initial Boundary
							posStart = 0;

							if(posEnd - posStart == boundaryLen && src.EqualTo(posStart, boundaryLen, boundary)) phase = 1;
							else if(posEnd - posStart == boundaryLen + 2 && src.EqualTo(posStart, boundaryLen, boundary) && src.EqualTo(posStart + boundaryLen, "--", 2)) phase = 5;
							else return HttpStatusCode::BAD_REQUEST;
							break;
						case 1:
							//Content-Disposition
							if(src.EqualTo(posStart, "Content-Disposition: ", 21))
							{
								posStart += 21;

								if(src.EqualTo(posStart, "form-data; ", 11)) posStart += 11;
								else if(src.EqualTo(posStart, "file; ", 6)) posStart += 6;
								else return HttpStatusCode::BAD_REQUEST;

								if(src.EqualTo(posStart, "name=\"", 6))
								{
									posStart += 6;

									size_t const posQuote = src.Find('\"', posStart);
									if(posQuote < posEnd && posQuote != PeoplezString::NPOS)
									{
										param.Name = src.Substring(posStart, posQuote - posStart);
										param.Name.Trim();

										if(!param.Name.IsUrlEncoded()) return HttpStatusCode::BAD_REQUEST;
									}
									else return HttpStatusCode::BAD_REQUEST;

									posStart = posQuote + 1;
								}
								else return HttpStatusCode::BAD_REQUEST;

								if(src.EqualTo(posStart, "filename=\"", 10))
								{
									posStart += 10;
									size_t const posQuote = src.Find('\"', posStart);
									if(posQuote < posEnd && posQuote != PeoplezString::NPOS)
									{
										param.FileName = src.Substring(posStart, posQuote - posStart);
										param.FileName.Trim();

										if(!param.FileName.IsUrlEncoded()) return HttpStatusCode::BAD_REQUEST;
									}
									else return HttpStatusCode::BAD_REQUEST;
								}

								++phase;
							}
							else return HttpStatusCode::BAD_REQUEST;
							break;
						case 2:
							//Content-Type or empty line
							if(posStart == posEnd) phase += 2;
							else if(src.EqualTo(posStart, "Content-Type: ", 14))
							{
								posStart += 14;
								param.Mime = MimeOperations::StringToMimeType(src.Substring(posStart, posEnd - posStart));
								if(param.Mime == MimeType::NONE) return HttpStatusCode::UNSUPPORTED_MEDIA_TYPE;
								++phase;
							}
							else return HttpStatusCode::BAD_REQUEST;
							break;
						case 3:
							//Empty line
							if(posStart == posEnd) ++phase;
							else return HttpStatusCode::BAD_REQUEST;
							break;
						case 4:
							//Content
							contentStart = posStart;
							++phase;
							break;
						case 5:
							//Content, Boundary or End
							if(posEnd - posStart == boundaryLen && src.EqualTo(posStart, boundaryLen, boundary)) phase = 1;
							else if(posEnd - posStart == boundaryLen  + 2 && src.EqualTo(posStart, boundaryLen, boundary)) phase = 6;

							if(phase != 5) dest.push_back(PostParam(name, src.Substring(contentStart, posStart - contentStart - 2)));
							break;
						}
					}

					Logger::LogEvent("end");

					if(phase != 6) return HttpStatusCode::BAD_REQUEST;
					else return HttpStatusCode::OK;
				}
				catch(...)
				{
					Logger::LogException("Error in HttpContext::HandleMultipartFormData", __FILE__, __LINE__);
				}

				return HttpStatusCode::INTERNAL_SERVER_ERROR;
			}

			void HttpContext::InsertHeader(PeoplezString const name, PeoplezString value) noexcept
			{
				try
				{
					value.TrimFast();

					switch(name.Length()) //With RETURN at the end of each complete match!!!
					{
//					case 4:
//						if(name == "Host")
//						{
//							request->userHostName = value;
//
//							return;
//						}
//						break;
					case 6:
						if(name == COMPARE_STRING_COOKIE)
						{
							std::vector<PeoplezString> cookies;
							value.Split<true>(cookies, ';');

							std::vector<PeoplezString>::size_type const cookiesSize = cookies.size();
							std::vector<PeoplezString>::size_type pos;

							for(std::vector<PeoplezString>::size_type i = 0; i < cookiesSize; ++i)
							{
								pos = cookies[i].Find('=');
								if(pos == PeoplezString::NPOS) throw "Unreadable cookie"; //TODOPeoplez So wirklich gut?
								PeoplezString firstPart = cookies[i].Substring(0, pos);
								PeoplezString secondPart = cookies[i].Substring(pos + 1, cookies[i].Length() - pos - 1);

								firstPart.Trim();
								secondPart.Trim();

								request.cookies.push_back(HttpCookie(firstPart, secondPart));
							}

							return;
						}
						break;
					case 10:
						if(name == COMPARE_STRING_CONNECTION)
						{
							value.ToLower_ASCII_NU();

							if(value.EqualTo("keep-alive", 10)) response.KeepAlive = request.keepAlive = true;
							else if(value.EqualTo("close", 5)) response.KeepAlive = request.keepAlive = false;
							else Logger::LogEvent("Connection not decodeable");

							return;
						}

						break;
					case 12:
						if(name.EqualTo("Content-Type", 12))
						{
							std::vector<PeoplezString> parts;
							value.Split<true>(parts, ';');

							if(parts.size())
							{
								request.contentType = MimeOperations::StringToMimeType(parts[0]);

								if(parts.size() > 1)
								{
									parts[1].Trim();

									if(parts[1].BeginsWith("boundary=", 9)) request.boundary = parts[1].Substring(9);
								}

								if(request.contentType == MimeType::MULTIPART_FORM_DATA && request.boundary.IsEmpty()) request.contentType = MimeType::NONE;
							}

							return;
						}
						break;
					case 13:
						if(name == COMPARE_STRING_IF_NONE_MATCH)
						{
							request.eTag = value.Substring(1, value.Length() - 2).ToUInt64(16);

							return;
						}
						break;
					case 14:
						if(name == COMPARE_STRING_CONTENT_LENGTH)
						{
							request.contentLength = value.ToInt64(10);

							return;
						}
						break;
					case 15:
						if(name == COMPARE_STRING_ACCEPT_LANGUAGE)
						{
							request.userLanguages = value;

							return;
						}
						break;
					default:
						break;
					}

					//request.headers[name] = value;
					request.headers.push_back(std::pair<PeoplezString, PeoplezString>(name, value));
				}
				catch(...)
				{
					Logger::LogException("Error in HttpContext::InsertHeader", __FILE__, __LINE__);
				}
			}

			void HttpContext::Reset()
			{
				OutputBuffer.Clear();
				request.Clean();
				response.Clean();
				//InputBuffer.Clear();
				Status = HTTP_SOCKET_STATUS_RECEIVE_HEADER;
			}

			/**
			 * @brief Destructor
			 */
			HttpContext::~HttpContext()
			{
				delete sender;
			}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
