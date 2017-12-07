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
#include "FileOperations.hpp"

namespace Peoplez
{
	namespace General
	{
		using namespace String;

		namespace FileOperations
		{
			PeoplezString FileTypeToFileExtension(FileType const type)
			{
				switch(type >> 16)
				{
				case FILE_TYPE::IMAGE >> 16:
					switch(type >> 8 & 0x0FF)
					{
					case FILE_TYPE::IMAGE_GRID >> 8 & 0x0FF:
						switch(type & 0x0FF)
						{
						case FILE_TYPE::IMAGE_BMP & 0x0FF:
							return "bmp";
							break;
						case FILE_TYPE::IMAGE_GIF & 0x0FF:
							return "gif";
							break;
						case FILE_TYPE::IMAGE_ICO & 0x0FF:
							return "ico";
							break;
						case FILE_TYPE::IMAGE_JPEG & 0x0FF:
							return "jpg";
							break;
						case FILE_TYPE::IMAGE_PIC & 0x0FF:
							return "pic";
							break;
						case FILE_TYPE::IMAGE_PNG & 0x0FF:
							return "png";
							break;
						case FILE_TYPE::IMAGE_PNM & 0x0FF:
							return "pnm";
							break;
						case FILE_TYPE::IMAGE_PSD & 0x0FF:
							return "psd";
							break;
						case FILE_TYPE::IMAGE_RAW & 0x0FF:
							return "raw";
							break;
						case FILE_TYPE::IMAGE_RGB & 0x0FF:
							return "rgb";
							break;
						case FILE_TYPE::IMAGE_TGA & 0x0FF:
							return "tga";
							break;
						case FILE_TYPE::IMAGE_TIFF & 0x0FF:
							return "tiff";
							break;
						}
						break;
					case FILE_TYPE::IMAGE_2D_VECTOR >> 8 & 0x0FF:
						switch(type & 0x0FF)
						{
						case FILE_TYPE::IMAGE_EPS & 0x0FF:
							return "eps";
							break;
						case FILE_TYPE::IMAGE_PS & 0x0FF:
							return "ps";
							break;
						case FILE_TYPE::IMAGE_SVG & 0x0FF:
							return "svg";
							break;
						case FILE_TYPE::IMAGE_SWF & 0x0FF:
							return "swf";
							break;
						case FILE_TYPE::IMAGE_WMF & 0x0FF:
							return "wmf";
							break;
						}
						break;
					case FILE_TYPE::IMAGE_3D_VECTOR >> 8 & 0x0FF:
						switch(type & 0x0FF)
						{
						case FILE_TYPE::IMAGE_BLEND & 0x0FF:
							return "blend";
							break;
						case FILE_TYPE::IMAGE_DXF & 0x0FF:
							return "dxf";
							break;
						case FILE_TYPE::IMAGE_IGS & 0x0FF:
							return "igs";
							break;
						case FILE_TYPE::IMAGE_STP & 0x0FF:
							return "stp";
							break;
						case FILE_TYPE::IMAGE_SAT & 0x0FF:
							return "sat";
							break;
						case FILE_TYPE::IMAGE_WRL & 0x0FF:
							return "wrl";
							break;
						}
						break;
					}
					break;
				case FILE_TYPE::TEXT >> 16:
					if(type == FILE_TYPE::TEXT_CSS) return "css";
					else if(type == FILE_TYPE::TEXT_HTML) return "html";
					else if((type & 0x0FF00) == (FILE_TYPE::TEXT_SCRIPT & 0x0FF00))
					{
						switch(type & 0x0FF)
						{
						case FILE_TYPE::TEXT_SCRIPT_JS & 0x0FF:
							return "js";
							break;
						case FILE_TYPE::TEXT_SCRIPT_PERL & 0x0FF:
							return "pl";
							break;
						case FILE_TYPE::TEXT_SCRIPT_PHP & 0x0FF:
							return "php";
							break;
						case FILE_TYPE::TEXT_SCRIPT_PYTHON & 0x0FF:
							return "py";
							break;
						case FILE_TYPE::TEXT_SCRIPT_RUBY & 0x0FF:
							return "rb";
							break;
						}
					}
					break;
				}

				return PeoplezString();
			}

			FileType FileExtensionToFileType(String::PeoplezString extension)
			{
				extension.ToLower();

				if(extension.Length() == 2)
				{
					if(extension.EqualTo("js", 2)) return FILE_TYPE::TEXT_SCRIPT_JS;
					else if(extension.EqualTo("ps", 2)) return FILE_TYPE::IMAGE_PS;
				}
				else if(extension.Length() > 2)
				{
					switch(extension[0])
					{
					case 'b':
						if(extension.EqualTo("bmp", 3)) return FILE_TYPE::IMAGE_BMP;
						else if(extension.EqualTo("blend", 5)) return FILE_TYPE::IMAGE_BLEND;
						break;
					case 'c':
						if(extension.EqualTo("css", 3)) return FILE_TYPE::TEXT_CSS;
						break;
					case 'd':
						if(extension.EqualTo("dxf", 3)) return FILE_TYPE::IMAGE_DXF;
						break;
					case 'e':
						if(extension.EqualTo("eps", 3)) return FILE_TYPE::IMAGE_EPS;
						break;
					case 'g':
						if(extension.EqualTo("gif", 3) || extension.EqualTo("giff", 4)) return FILE_TYPE::IMAGE_GIF;
						break;
					case 'h':
						if(extension.EqualTo("html", 4)) return FILE_TYPE::TEXT_HTML;
						break;
					case 'i':
						if(extension.EqualTo("ico", 3)) return FILE_TYPE::IMAGE_ICO;
						else if(extension.EqualTo("igs", 3)) return FILE_TYPE::IMAGE_IGS;
						break;
					case 'j':
						if(extension.EqualTo("jpg", 3) || extension.EqualTo("jpeg", 4) || extension.EqualTo("jpe", 3) || extension.EqualTo("jfif", 4)) return FILE_TYPE::IMAGE_JPEG;
						break;
					case 'p':
						if(extension[1] == 'n')
						{
							if(extension.EqualTo("png", 3)) return FILE_TYPE::IMAGE_PNG;
							else if(extension.EqualTo("pnm", 3)) return FILE_TYPE::IMAGE_PNM;
						}
						else if(extension.EqualTo("pic", 3)) return FILE_TYPE::IMAGE_PIC;
						else if(extension.EqualTo("psd", 3)) return FILE_TYPE::IMAGE_PSD;
						break;
					case 'r':
						if(extension.EqualTo("raw", 3)) return FILE_TYPE::IMAGE_RAW;
						else if(extension.EqualTo("rgb", 3) || extension.EqualTo("rgba", 4) || extension.EqualTo("sgi", 3)) return FILE_TYPE::IMAGE_RGB;
						break;
					case 's':
						if(extension.EqualTo("sat", 3)) return FILE_TYPE::IMAGE_SAT;
						else if(extension.EqualTo("stp", 3)) return FILE_TYPE::IMAGE_STP;
						else if(extension.EqualTo("svg", 3)) return FILE_TYPE::IMAGE_SVG;
						else if(extension.EqualTo("swf", 3)) return FILE_TYPE::IMAGE_SWF;
						break;
					case 't':
						if(extension.EqualTo("tga", 3) || extension.EqualTo("bpx", 3) || extension.EqualTo("icb", 3) || extension.EqualTo("pix", 3)) return FILE_TYPE::IMAGE_TGA;
						else if(extension.EqualTo("tif", 3) || extension.EqualTo("tiff", 4)) return FILE_TYPE::IMAGE_TIFF;
						break;
					case 'w':
						if(extension.EqualTo("wmf", 3)) return FILE_TYPE::IMAGE_WMF;
						else if(extension.EqualTo("wrl", 3) || extension.EqualTo("wrz", 3)) return FILE_TYPE::IMAGE_WRL;
						break;
					}
				}

				return FILE_TYPE::NONE;
			}
		} // namespace FileOperations
	} // namespace General
} // namespace Peoplez
