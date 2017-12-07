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
#include "HttpClientInfo.hpp"

// Local includes
#include "../../System/Logging/Logger.hpp"
#include "../../System/Resources/ResourceManager.hpp"
#include "../../String/PeoplezString.hpp"

/**
 * @def MAX_HEADER_LENGTH
 * @brief Maximum accepted length of the http header in bytes
 */
#define MAX_HEADER_LENGTH 16384
/**
 * @def MAX_BODY_LENGTH
 * @brief Maximum accepted length of the http body in bytes
 */
#define MAX_BODY_LENGTH 5242880
/**
 * @def INPUT_BUFFER_STEP_SIZE
 * @brief Size of the read buffer in bytes
 * @details Number of bytes that can be read in one step. To read on the buffer must be appended to the target string first.
 */
#define INPUT_BUFFER_STEP_SIZE 2000

namespace Peoplez
{
	// Loca namespaces
	using namespace String;
	using namespace System::Logging;

	namespace Services
	{
		namespace Http
		{
			HttpClientInfo::HttpClientInfo(int const fileDescriptor, HttpRequestHandler & reqHandler, System::IO::Network::Socket * const _sender)
				: ClientInfo(fileDescriptor >= 0 ? fileDescriptor : throw std::invalid_argument("Invalid file descriptor")), requestHandler(reqHandler), context(new HttpContext(_sender)), firstByte(0)
			{
			}

			/*
			HttpClientInfo::HttpClientInfo(const HttpClientInfo& other) noexcept : ClientInfo(other), pageMgr(other.pageMgr), context(other.context)
			{
			}
			*/

			void HttpClientInfo::BodyReceived()
			{
				try
				{
					if(context->InputBuffer.Length() < context->request.ContentLength()) Logger::LogException((PeoplezString)"Body not completely received: " + PeoplezString::ParseDec(context->InputBuffer.Length()) + " von " + PeoplezString::ParseDec(context->request.ContentLength()), __FILE__, __LINE__);
					else
					{
						HttpStatusCode stat = context->HandleBody(context->request.ContentLength());

						if(context->InputBuffer.Length() > context->request.ContentLength()) context->InputBuffer = context->InputBuffer.Substring(context->request.ContentLength());
						else context->InputBuffer.Clear();

						if(stat == HttpStatusCode::OK) MessageReady();
						else
						{
							context->response.SetOther(stat);
							context->OutputBuffer = context->response.GetResponseText();
							SwitchToSend();
						}
					}
				}
				catch(...)
				{
					Logger::LogException("Error in HttpClientInfo::BodyReceived", __FILE__, __LINE__);
				}
			}

			void HttpClientInfo::HeaderReceived(size_t const size)
			{
				try
				{
					//Call handler
					HttpStatusCode const stat = context->HandleHeader();
					//Clean-Up
					if(context->InputBuffer.Length() > size + 4) context->InputBuffer = context->InputBuffer.Substring(size + 4);
					else context->InputBuffer.Clear();

					if(stat == HttpStatusCode::OK) //If header is ok
					{
						if(context->request.HttpMethod() == HttpMethods::GET || context->request.HttpMethod() == HttpMethods::POST || context->request.HttpMethod() == HttpMethods::PUT) //If method is supported
						{
							if(context->request.ContentLengthIsSet()) //If body is needed
							{
								if(context->request.ContentLength() > MAX_BODY_LENGTH) //If message is too long
								{
									Logger::LogEvent("Request too long");
									context->response.SetOther(HttpStatusCode::REQUEST_ENTITY_TOO_LARGE);
									context->response.KeepAlive = false;
									context->OutputBuffer = context->response.GetResponseText();
									SwitchToSend();
								}
								else if(context->request.ContentLength() > context->InputBuffer.Length()) //If body is not completely received
								{
									context->Status = HTTP_SOCKET_STATUS_RECEIVE_BODY;

									if(context->request.ContentLength() > 2 * INPUT_BUFFER_STEP_SIZE) context->InputBuffer.Resize(context->request.ContentLength());
								}
								else BodyReceived(); //If body is completely received ... handle it
							}
							else if(context->request.HttpMethod() == HttpMethods::POST || context->request.HttpMethod() == HttpMethods::PUT) //If body would be needed
							{
								context->response.SetOther(HttpStatusCode::LENGTH_REQUIRED);
								context->response.KeepAlive = false;
								context->OutputBuffer = context->response.GetResponseText();
								SwitchToSend();
							}
							else MessageReady();
						}
						else
						{
							context->response.SetOther(HttpStatusCode::METHOD_NOT_ALLOWED);
							context->response.KeepAlive = false;
							context->OutputBuffer = context->response.GetResponseText();
							SwitchToSend();
						}
					}
					else
					{
						context->response.SetOther(stat);
						context->response.KeepAlive = false;
						context->OutputBuffer = context->response.GetResponseText();
						SwitchToSend();
					}
				}
				catch(...)
				{
					Logger::LogException("Error in HttpClientInfo::HeaderReceived", __FILE__, __LINE__);
				}
			}

			void HttpClientInfo::MessageReady()
			{
				try
				{
					requestHandler.ProcessRequest(*context.get());

					context->OutputBuffer = context->response.GetResponseText();
					SwitchToSend();
				}
				catch(...)
				{
					Logger::LogException("Error in HttpClientInfo::MessageReady", __FILE__, __LINE__);
				}
			}

			void HttpClientInfo::MessageReceivableCB()
			{
				// fd >= 0 ensured by constructor

				try
				{
					char buf[INPUT_BUFFER_STEP_SIZE];

					boost::unique_lock<boost::mutex> const lock(context->mut);

					if(context->InputBuffer.IsEmpty()) firstByte = time(0);
					else
					{
						uint64_t const diffTime = time(0) - firstByte;
						uint64_t const minData = (8192 * (diffTime ? diffTime - 1 : 0));

						if(__builtin_expect(context->InputBuffer.Length() < minData, false))
						{
							context->response.SetOther(HttpStatusCode::REQUEST_TIMEOUT);
							context->OutputBuffer = context->response.GetResponseText();
							SwitchToSend();
						}
					}

					for(unsigned int i = (MAX_HEADER_LENGTH + MAX_BODY_LENGTH)/INPUT_BUFFER_STEP_SIZE; i > 0 && context->sender->IsOpen(); --i)
					{
						int const bytes = context->sender->Recv(buf, INPUT_BUFFER_STEP_SIZE);

						if(bytes > 0)
						{
							size_t startPos = context->InputBuffer.Length();
							context->InputBuffer.Append(buf, bytes);

							if(context->Status == HTTP_SOCKET_STATUS_SEND)
							{
								context->Status = HTTP_SOCKET_STATUS_RECEIVE_HEADER;
								Logger::LogException("Received in send mode", __FILE__, __LINE__);
							}

							if(context->Status == HTTP_SOCKET_STATUS_RECEIVE_HEADER)
							{
								size_t const dlPos = context->InputBuffer.FindDoubleNewLine(startPos > 4 ? startPos - 4 : 0);
								if(dlPos != String::PeoplezString::NPOS) HeaderReceived(dlPos);
								else if(context->InputBuffer.Length() > MAX_HEADER_LENGTH)
								{
									Logger::LogEvent("Request too long");
									context->response.SetOther(HttpStatusCode::REQUEST_ENTITY_TOO_LARGE);
									context->response.KeepAlive = false;
									context->OutputBuffer = context->response.GetResponseText();
									SwitchToSend();
								}
							}
							else if(context->Status == HTTP_SOCKET_STATUS_RECEIVE_BODY)
							{
								Logger::LogEvent(PeoplezString::ParseDec(context->InputBuffer.Length()) + " von " + PeoplezString::ParseDec(context->request.ContentLength()));
								//if(context->request.ContentLength() <= (size_t)bytes + context->InputBuffer.Length()) BodyReceived();
								if(context->request.ContentLength() <= context->InputBuffer.Length())
								{
									Logger::LogEvent("BodyReceived");
									BodyReceived();
								}
							}
							else Logger::LogException("Received in unknown mode", __FILE__, __LINE__);
						}
						else if(bytes < 0)
						{
							if(errno != EAGAIN)
							{
								Logger::LogException("Error while reading http request", __FILE__, __LINE__);
								Logger::LogException(strerror(errno), __FILE__, __LINE__);
							}
							break;
						}
#ifdef SHOW_RB_EMPTY
						else
						{
							Logger::LogEvent("Read buffer empty");
							break;
						}
#endif
					}
				}
				catch(...)
				{
					//context->response.SetOther(INTERNAL_SERVER_ERROR);
					Logger::LogException("Error in HttpClientInfo::MessageReceiveCB", __FILE__, __LINE__);
				}
			}

//			void HttpClientInfo::MessageReceivableCB2()
//			{
//				if(__builtin_expect(fd < 0, false)) return;
//
//				try
//				{
//					int bytes;
//					char buf[INPUT_BUFFER_STEP_SIZE];
//
//					boost::unique_lock<boost::mutex> lock(context->mut);
//
//					bytes = context->sender->Recv(buf, INPUT_BUFFER_STEP_SIZE);
//
//					if(bytes > 0)
//					{
//						context->InputBuffer.Append(buf, bytes);
//
//
//					}
//					else if(bytes < 0)
//					{
//						if(errno != EAGAIN)
//						{
//							Logger::LogException("Error while reading http request", __FILE__, __LINE__);
//							Logger::LogException(strerror(errno), __FILE__, __LINE__);
//						}
//						//break;
//					}
//					else
//					{
//						Logger::LogEvent("Read buffer empty");
//						//break;
//					}
//				}
//				catch(...)
//				{
//					//context->response.SetOther(INTERNAL_SERVER_ERROR);
//					Logger::LogException("Error in HttpClientInfo::MessageReceiveCB", __FILE__, __LINE__);
//				}
//			}

			void HttpClientInfo::MessageSendableCB()
			{
				boost::unique_lock<boost::mutex> const lock(context->mut);
				SendInner();
			}

			void HttpClientInfo::SendInner()
			{
				try
				{
					if(!context->OutputBuffer.Length()) return;

					int const sent = context->sender->Send(context->OutputBuffer.GetData(), context->OutputBuffer.Length());

					if(__builtin_expect(sent < 0, false)) Logger::LogEvent("Error while writing");
					else if((size_t) sent >= context->OutputBuffer.Length())
					{
						bool const keepAlive = context->response.KeepAlive;

						context->Reset();
						context->Status = HTTP_SOCKET_STATUS_RECEIVE_HEADER;

						if(!keepAlive)
						{
							context->sender->Close();
						}
					}
					else context->OutputBuffer <<= sent;
				}
				catch(...)
				{
					Logger::LogException("Error in HttpClientInfo::SendInner", __FILE__, __LINE__);
				}
			}

			void HttpClientInfo::SwitchToSend()
			{
				context->Status = HTTP_SOCKET_STATUS_SEND;
				SendInner();

//				if(context->Status == HTTP_SOCKET_STATUS_SEND)
//				{
//					Modify(true, true);
//					context->SendableCBEnabled = true;
//				}
			}

//			bool HttpClientInfo::IsSecureConnection()
//			{
//				return secureConnection;
//			}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
