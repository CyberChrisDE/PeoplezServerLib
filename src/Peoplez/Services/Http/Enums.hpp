/**
 * Copyright 2017,2018 Christian Geldermann
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

#ifndef PEOPLEZ_SERVICES_HTTP_ENUMS_H_
#define PEOPLEZ_SERVICES_HTTP_ENUMS_H_

namespace Peoplez
{
	namespace Services
	{
		namespace Http
		{
			/**
			 * State of http receiving/sending
			 */
			enum HttpSocketStatus
			{
				HTTP_SOCKET_STATUS_RECEIVE_HEADER,
				HTTP_SOCKET_STATUS_RECEIVE_BODY,
				HTTP_SOCKET_STATUS_SEND
			};

			/**
			 * @brief Http methods
			 */
			enum class HttpMethods
			{
				OPTIONS,
				GET,
				HEAD,
				POST,
				PUT,
				PATCH,
				DELETE,
				TRACE,
				CONNECT,
				UNKNOWN
			};

			/**
			 * @brief Http compression variants
			 */
			enum HttpCompression
			{
				HTTP_COMPRESSION_GZIP,
				HTTP_COMPRESSION_DEFLATE,
				HTTP_COMPRESSION_NONE
			};
		} // namespace Http
	} // namespace Services
} // namespace Peoplez

#endif // PEOPLEZ_SERVICES_HTTP_ENUMS_H_
