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

			void HttpClientInfo::DataReceived(uint64_t bytesReceived)
			{
				size_t startPos = context->InputBuffer.Length() - bytesReceived;
				// If waiting for end of header ...
				// Else if waiting for end of body ...
				// Else if not waiting for input ...
				// Else log error
				if(context->Status == HTTP_SOCKET_STATUS_RECEIVE_HEADER)
				{
					// Search for end of header
					size_t const dlPos = context->InputBuffer.FindDoubleNewLine(startPos > 4 ? startPos - 4 : 0);

					// If end of header was found ...
					// Else if request is too long ...
					if(dlPos != String::PeoplezString::NPOS)
					{
						// Read headers
						HeaderReceived(dlPos);
					}
					else if(context->InputBuffer.Length() > MAX_HEADER_LENGTH)
					{
						// Log it
						Logger::LogEvent("Request too long");

						// Set error response
						context->response.SetOther(HttpStatusCode::REQUEST_ENTITY_TOO_LARGE);

						// And initiate disconnect
						context->response.KeepAlive = false;

						// Send answer (error)
						context->OutputBuffer = context->response.GetResponseText();
						SwitchToSend();
					}
				}
				else if(context->Status == HTTP_SOCKET_STATUS_RECEIVE_BODY)
				{
					// Debug log of bytes received
					Logger::LogEvent(PeoplezString::ParseDec(context->InputBuffer.Length()) + " von " + PeoplezString::ParseDec(context->request.ContentLength()));

					// Check whether body is completely received
					if(context->request.ContentLength() <= context->InputBuffer.Length())
					{
						// Debug log
						Logger::LogEvent("BodyReceived");

						// Handle body
						BodyReceived();
					}
				}
				else if(context->Status == HTTP_SOCKET_STATUS_SEND)
				{
					//context->Status = HTTP_SOCKET_STATUS_RECEIVE_HEADER;
					//Logger::LogException("Received in send mode", __FILE__, __LINE__);
					//TODO Attacker could send huge amounts of data here
				}
				else Logger::LogException("Received in unknown mode", __FILE__, __LINE__);
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
					// Reserve memory for receiving
					char buf[INPUT_BUFFER_STEP_SIZE];

					// Lock the context while receiving
					std::unique_lock<std::mutex> const lock(context->mut);

					// If no bytes received so far ...
					if(context->InputBuffer.IsEmpty()) firstByte = time(0);
					else
					{
						// Prevent Slow Loris attacks
						uint64_t const diffTime = time(0) - firstByte;
						uint64_t const minData = (8192 * (diffTime ? diffTime - 1 : 0));

						if(__builtin_expect(context->InputBuffer.Length() < minData, false))
						{
							// Set error response
							context->response.SetOther(HttpStatusCode::REQUEST_TIMEOUT);

							// And initiate disconnect
							context->response.KeepAlive = false;

							// Send answer (error)
							context->OutputBuffer = context->response.GetResponseText();
							SwitchToSend();
						}
					}

					for(unsigned int i = (MAX_HEADER_LENGTH + MAX_BODY_LENGTH)/INPUT_BUFFER_STEP_SIZE; i > 0 && context->sender->IsOpen(); --i)
					{
						// Receive next chunk of data
						int const bytes = context->sender->Recv(buf, INPUT_BUFFER_STEP_SIZE);

						if(bytes > 0) // If sth. received ...
						{
							// Append received chunk to input buffer
							context->InputBuffer.Append(buf, bytes);

							// Handle received data
							DataReceived(bytes);
						}
						else if(bytes < 0) // On error ...
						{
							// Log error (if not EAGAIN)
							if(errno != EAGAIN)
							{
								Logger::LogException("Error while reading http request", __FILE__, __LINE__);
								Logger::LogException(strerror(errno), __FILE__, __LINE__);
							}

							// Stop receiving
							break;
						}
#ifdef SHOW_RB_EMPTY
						else
						{
							// Log the fact that no bytes were received
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

			void HttpClientInfo::MessageSendableCB()
			{
				std::unique_lock<std::mutex> const lock(context->mut);
				SendInner();
			}

			void HttpClientInfo::SendInner()
			{
				try
				{
					// If output buffer is empty ... return
					if(!context->OutputBuffer.Length()) return;

					// Send remaining content in output buffer
					int const sent = context->sender->Send(context->OutputBuffer.GetData(), context->OutputBuffer.Length());

					// If an error occured while sending ...
					//   Log an error
					// Else if everything could be sent ...
					// Else ...
					//   Remove sent content from output buffer
					if(__builtin_expect(sent < 0, false)) Logger::LogEvent("Error while writing");
					else if((size_t) sent >= context->OutputBuffer.Length())
					{
						// Backup keep alive flag
						bool const keepAlive = context->response.KeepAlive;

						// Clean up http context and switch to receive header mode
						context->Reset();
						context->Status = HTTP_SOCKET_STATUS_RECEIVE_HEADER;

						// If not keep alive ...
						// 	 Close socket
						if(!keepAlive) context->sender->Close();
						else DataReceived(context->InputBuffer.Length());
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
			}
		} // namespace Http
	} // namespace Services
} // namespace Peoplez
