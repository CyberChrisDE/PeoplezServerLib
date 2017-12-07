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

// Own Headers
#include "MimeOperations.hpp"

namespace Peoplez
{
	namespace General
	{
		using namespace String;

		namespace MimeOperations
		{
			PeoplezString MimeTypeToString(MimeType const mime)
			{
				switch((unsigned int)MimeContentTypeOf(mime) >> 16)
				{
				case (unsigned int)MimeType::APPLICATION >> 16:
					break;
				case (unsigned int)MimeType::AUDIO >> 16:
					break;
				case (unsigned int)MimeType::EXAMPLE >> 16:
					break;
				case (unsigned int)MimeType::IMAGE >> 16:
					break;
				case (unsigned int)MimeType::MESSAGE >> 16:
					break;
				case (unsigned int)MimeType::MODEL >> 16:
					break;
				case (unsigned int)MimeType::MULTIPART >> 16:
					break;
				case (unsigned int)MimeType::TEXT >> 16:
					break;
				case (unsigned int)MimeType::VIDEO >> 16:
					break;
				default:
					return PeoplezString();
				}
				//TODOPeoplez Implementieren

				return PeoplezString();
			}

			MimeType StringToMimeType(PeoplezString mime)
			{
				mime.ToLower();
				size_t const len = mime.Length();

				if(len >= 7)
				{
					switch(mime[0])
					{
					case 'a':
						if(len >= 14 && mime.BeginsWith("application/", 12))
						{
							if(len == 33 && mime.EqualTo(12, "x-www-form-urlencoded", 21)) return MimeType::APPLICATION_X_WWW_FORM_URLENCODED;
						}
						else if(len >= 8 && mime.BeginsWith("audio/", 6))
						{
							if(mime[6] == 'm')
							{
								if(len == 9 && mime.EqualTo(7, "p4", 2)) return MimeType::AUDIO_MP4;
								else if(len == 10 && mime.EqualTo(7, "peg", 3)) return MimeType::AUDIO_MPEG;
								else if(len == 19 && mime.EqualTo(7, "peg4-generic", 12)) return MimeType::AUDIO_MPEG4_GENERIC;
							}
							else if(len == 9 && mime.EqualTo(6, "ogg", 3)) return MimeType::AUDIO_OGG;
						}
						break;
					case 'e':
						if(mime.EqualTo("example", 7)) return MimeType::EXAMPLE;
						break;
					case 'i':
						if(len >= 9 && mime.BeginsWith("image/", 6))
						{
							switch(mime[6])
							{
							case 'e':
								if(len == 13 && mime.EqualTo(7, "xample", 6)) return MimeType::IMAGE_EXAMPLE;
								break;
							case 'g':
								if(len == 9 && mime.EqualTo(7, "if", 2)) return MimeType::IMAGE_GIF;
								break;
							case 'i':
								if(len == 9 && mime.EqualTo(7, "ef", 2)) return MimeType::IMAGE_IEF;
								break;
							case 'j':
								if(len == 9 && mime.EqualTo(7, "p2", 2)) return MimeType::IMAGE_JP2;
								else if(len == 10 && mime.EqualTo(7, "peg", 3)) return MimeType::IMAGE_JPEG;
								break;
							case 'p':
								if(len == 9 && mime.EqualTo(7, "ng", 2)) return MimeType::IMAGE_PNG;
								break;
							case 't':
								if(len == 10 && mime.EqualTo(7, "iff", 3)) return MimeType::IMAGE_TIFF;
								else if(len == 13 && mime.EqualTo(7, "iff-fx", 6)) return MimeType::IMAGE_TIFF_FX;
								break;
							}
						}
						break;
					case 'm':
						if(len >= 15 && mime.BeginsWith("multipart/", 10))
						{
							switch(mime[10])
							{
							case 'a':
								if(len == 21 && mime.EqualTo(11, "lternative", 10)) return MimeType::MULTIPART_ALTERNATIVE;
								break;
							case 'b':
								if(len == 20 && mime.EqualTo(11, "yteranges", 9)) return MimeType::MULTIPART_BYTERANGES;
								break;
							case 'd':
								if(len == 16 && mime.EqualTo(11, "igest", 5)) return MimeType::MULTIPART_DIGEST;
								break;
							case 'e':
								if(len == 19 && mime.EqualTo(11, "ncrypted", 8)) return MimeType::MULTIPART_ENCRYPTED;
								else if(len == 17 && mime.EqualTo(11, "xample", 6)) return MimeType::MULTIPART_EXAMPLE;
								break;
							case 'f':
								if(len == 19 && mime.EqualTo(11, "orm-data", 8)) return MimeType::MULTIPART_FORM_DATA;
								break;
							case 'm':
								if(len == 15 && mime.EqualTo(11, "ixed", 4)) return MimeType::MULTIPART_MIXED;
								break;
							case 's':
								if(len == 16 && mime.EqualTo(11, "igned", 5)) return MimeType::MULTIPART_SIGNED;
								break;
							}
						}
						else if(len >= 11 && mime.BeginsWith("message/", 8))
						{
							if(len == 15 && mime.EqualTo(8, "example", 7)) return MimeType::MESSAGE_EXAMPLE;
							else if(len == 21 && mime.EqualTo(8, "external-body", 13)) return MimeType::MESSAGE_EXTERNAL_BODY;
							else if(len == 12 && mime.EqualTo(8, "http", 4)) return MimeType::MESSAGE_HTTP;
							else if(len == 15 && mime.EqualTo(8, "partial", 7)) return MimeType::MESSAGE_PARTIAL;
							else if(len == 14 && mime.EqualTo(8, "rfc822", 6)) return MimeType::MESSAGE_RFC822;
							else if(len == 14 && mime.EqualTo(8, "s-http", 6)) return MimeType::MESSAGE_S_HTTP;
							else if(len == 11 && mime.EqualTo(8, "sip", 3)) return MimeType::MESSAGE_SIP;
						}
						else if(len >= 10 && mime.BeginsWith("model/", 6))
						{
							if(len == 13 && mime.EqualTo(6, "example", 7)) return MimeType::MODEL_EXAMPLE;
							else if(len == 10 && mime.EqualTo(6, "mesh", 4)) return MimeType::MODEL_MESH;
						}
						break;
					case 't':
						if(len >= 8 && mime.BeginsWith("text/", 5))
						{
							if(mime.EqualTo(5, "calendar", 8)) return MimeType::TEXT_CALENDAR;
							else if(mime.EqualTo(5, "css", 3)) return MimeType::TEXT_CSS;
							else if(mime.EqualTo(5, "csv", 3)) return MimeType::TEXT_CSV;
							else if(mime.EqualTo(5, "dns", 3)) return MimeType::TEXT_DNS;
							else if(mime.EqualTo(5, "example", 7)) return MimeType::TEXT_EXAMPLE;
							else if(mime.EqualTo(5, "html", 4)) return MimeType::TEXT_HTML;
							else if(mime.EqualTo(5, "plain", 5)) return MimeType::TEXT_PLAIN;
							else if(mime.EqualTo(5, "rfc822-headers", 14)) return MimeType::TEXT_RFC822_HEADERS;
							else if(mime.EqualTo(5, "richtext", 8)) return MimeType::TEXT_RICHTEXT;
							else if(mime.EqualTo(5, "rtf", 3)) return MimeType::TEXT_RTF;
							else if(mime.EqualTo(5, "sml", 3)) return MimeType::TEXT_SML;
						}
						break;
					case 'v':
						if(len >= 9 && mime.BeginsWith("video/", 6))
						{
							if(mime.EqualTo(6, "h264", 4)) return MimeType::VIDEO_H264;
							else if(mime.EqualTo(6, "jpeg", 4)) return MimeType::VIDEO_JPEG;
							else if(mime.EqualTo(6, "mp4", 3)) return MimeType::VIDEO_MP4;
							else if(mime.EqualTo(6, "mpeg", 4)) return MimeType::VIDEO_MPEG;
							else if(mime.EqualTo(6, "mpeg4-generic", 13)) return MimeType::VIDEO_MPEG4_GENERIC;
							else if(mime.EqualTo(6, "ogg", 3)) return MimeType::VIDEO_OGG;
							else if(mime.EqualTo(6, "quicktime", 9)) return MimeType::VIDEO_QUICKTIME;
						}
						break;
					}
				}

				return MimeType::NONE;
			}
		} // namespace MimeOperations
	} // namespace General
} // namespace Peoplez
