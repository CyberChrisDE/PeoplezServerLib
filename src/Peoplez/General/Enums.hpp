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

#ifndef ENUMS_H_
#define ENUMS_H_

namespace LANGUAGE
{
	/**
	 * @brief Human languages
	 */
	enum Language
	{
		MIN = 0, //< Minimum Language value
		DE = 0,
		EN,
		FR,
		MAX = FR, //< Maximum Language value
		COUNT = 3 //< Number of languages in enum Language
	};
}

/**
 * @var typedef LANGUAGE::Language Language
 * @brief Brings Language to this namespace
 */
typedef LANGUAGE::Language Language;

namespace FILE_TYPE
{
	/**
	 * @brief Enum FileType
	 *
	 * @details Enum FileType with bit masks inside:
	 * Bits 16-32:	File category
	 * Bits 8-15:	File sub category
	 * Bits 0-7:	Specific file type
	 */
	enum FileType
	{
		MIN = 0,
		NONE = 0,
		IMAGE = 0x010000,				/*!< Bit mask for graphic types */
		IMAGE_GRID = 0x010100,		/*!< Bit mask for grid graphic types */
		IMAGE_BMP,
		IMAGE_GIF,
		IMAGE_ICO,
		IMAGE_JPEG,
		IMAGE_PIC,
		IMAGE_PNG,
		IMAGE_PNM,
		IMAGE_PSD,
		IMAGE_RAW,
		IMAGE_RGB,
		IMAGE_TGA,
		IMAGE_TIFF,
		IMAGE_2D_VECTOR = 0x010200,	/*!< Bit mask for 2D vector graphic types */
		IMAGE_EPS,
		IMAGE_PS,
		IMAGE_SVG,
		IMAGE_SWF,
		IMAGE_WMF,
		IMAGE_3D_VECTOR = 0x010300,	/*!< Bit mask for 3D vector graphic types */
		IMAGE_BLEND,
		IMAGE_DXF,
		IMAGE_IGS,
		IMAGE_STP,
		IMAGE_SAT,
		IMAGE_WRL,
		TEXT = 0x020000,				/*!< Bit mask for text file types */
		TEXT_CSS,
		TEXT_HTML,
		TEXT_SCRIPT = 0x020100,		/*!< Bit mask for script file types */
		TEXT_SCRIPT_JS,
		TEXT_SCRIPT_PERL,
		TEXT_SCRIPT_PHP,
		TEXT_SCRIPT_PYTHON,
		TEXT_SCRIPT_RUBY,
		EXEC = 0x030000,				/*!< Bit mask for executable file types */
		MAX = EXEC
	};
}

/**
 * @var typedef FILE_TYPE::FileType FileType
 * @brief Brings FileType to this namespace
 */
typedef FILE_TYPE::FileType FileType;

/**
 * @def IsFileType(type) (((unsigned int)type & 0xFF) != 0)
 * @brief Determines wheter the given type is a specific file type
 */
#define IsFileType(type) (((unsigned int)type & 0xFF) != 0)
/**
 * @def IsCategoryOf(ChildType, ParentType) ((ChildType & 0xFFFF0000) == ParentType)
 * @brief Determins wheter the ChildType is a specific file type of the ParentType
 */
#define IsCategoryOf(ChildType, ParentType) ((ChildType & 0xFFFF0000) == ParentType)
/**
 * @def FileCategoryOf(type) (FileType)((unsigned int)type & ~0xFFFF)
 * @brief Determines the file category of the file type
 */
#define FileCategoryOf(type) (FileType)((unsigned int)type & ~0xFFFF)
/**
 * @def SubFileCategoryOf(type) (FileType)((unsigned int)type & ~0xFF)
 * @brief Determines the sub file type of the file type
 */
#define SubFileCategoryOf(type) (FileType)((unsigned int)type & ~0xFF)

/**
 * @brief Enum MimeType
 * @details Enum MimeType with bit masks inside:
 * Bits 16-32:	Content types
 * Bits 0-16:	Sub types
 */
enum class MimeType : unsigned int
{
	NONE = 0x0,
	APPLICATION = 0x010000,
		APPLICATION_X_WWW_FORM_URLENCODED,
	AUDIO = 0x020000,
		AUDIO_MP4,
		AUDIO_MPEG,
		AUDIO_MPEG4_GENERIC,
		AUDIO_OGG,
	EXAMPLE = 0x030000,
	IMAGE = 0x040000,
		IMAGE_EXAMPLE,
		IMAGE_GIF,
		IMAGE_IEF,
		IMAGE_JP2,
		IMAGE_JPEG,
		IMAGE_PNG,
		IMAGE_TIFF,
		IMAGE_TIFF_FX,
	MESSAGE = 0x050000,
		MESSAGE_EXAMPLE,
		MESSAGE_EXTERNAL_BODY,
		MESSAGE_HTTP,
		MESSAGE_PARTIAL,
		MESSAGE_RFC822,
		MESSAGE_S_HTTP,
		MESSAGE_SIP,
	MODEL = 0x060000,
		MODEL_EXAMPLE,
		MODEL_MESH,
	MULTIPART = 0x070000,
		MULTIPART_ALTERNATIVE,
		MULTIPART_BYTERANGES,
		MULTIPART_DIGEST,
		MULTIPART_ENCRYPTED,
		MULTIPART_EXAMPLE,
		MULTIPART_FORM_DATA,
		MULTIPART_MIXED,
		MULTIPART_SIGNED,
	TEXT = 0x080000,
		TEXT_CALENDAR,
		TEXT_CSS,
		TEXT_CSV,
		TEXT_DNS,
		TEXT_EXAMPLE,
		TEXT_HTML,
		TEXT_PLAIN,
		TEXT_RFC822_HEADERS,
		TEXT_RICHTEXT,
		TEXT_RTF,
		TEXT_SML,
	VIDEO = 0x090000,
		VIDEO_H264,
		VIDEO_JPEG,
		VIDEO_MP4,
		VIDEO_MPEG,
		VIDEO_MPEG4_GENERIC,
		VIDEO_OGG,
		VIDEO_QUICKTIME
};

/**
 * @def IsMimeContentType(Subtype, ContentType) ((SubType & 0xFFFF0000) == ContentType)
 * @brief Determines whether SubType is a sub type of ContentType
 */
#define IsMimeContentType(SubType, ContentType) ((SubType & 0xFFFF0000) == ContentType)
/**
 * @def MimeContentTypeOf(type) (MimeType)((unsigned int)type & ~0xFFFF)
 * @brief Determines the content type of a specific mime type
 */
#define MimeContentTypeOf(type) (MimeType)((unsigned int)type & ~0xFFFF)

enum FileSaveStatus
{
	FILE_SAVE_STATUS_OK,
	FILE_SAVE_STATUS_DUPLICATE,
	FILE_SAVE_STATUS_ERROR
};

#endif // ENUMS_H_
