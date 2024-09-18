/**
 * Copyright 2017 - 2020, 2024 Christian Geldermann
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

// Own Header
#include "PeoplezString.hpp"

// Local includes
#include "../System/Alignment.hpp"
#include "../String/Parsing/IntToString.hpp"

// External includes
#include <bit>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <ostream>
#include <zlib.h>

//typedef unsigned char v8uc __attribute__((vector_size(8)));
//typedef char v8sc __attribute__((vector_size(8)));

// External namespaces
using namespace std;

namespace Peoplez
{
	// Local namespaces
	using namespace System;

	namespace String
	{
		// Local namespaces
		using namespace Parsing;

		const size_t PeoplezString::NPOS = -1;
		const size_t PeoplezString::COPIES_SIZE = sizeof(uint64_t);
		unsigned char const PeoplezString::DEC2HEX[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
		unsigned char const PeoplezString::HEX2DEC[256] =
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
				0, 0, 0, 0, 0, 0, 0, 55, 55, 55, 55, 55, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 87, 87, 87, 87, 87,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		char const PeoplezString::zeroTermination = '\0';
		uint64_t const PeoplezString::UrlEncBitmap[2] = {0xFC00FFFFFFFFFFFF, 0xF8000001F8000001};

		PeoplezString::PeoplezString(size_t const reservation) noexcept(false) : dataLen(0), copies((COUNTER *) NEW(reservation + COPIES_SIZE)), data(((char *)copies) + COPIES_SIZE), reservedBytes(reservation)
		{
			*copies = 0;
		}

		PeoplezString::PeoplezString(char const * const __restrict__ str) noexcept(false) : dataLen(strlen(str)), copies((COUNTER *) NEW(dataLen + COPIES_SIZE)), data(((char *)copies) + COPIES_SIZE), reservedBytes(dataLen)
		{
			*copies = 0;
			memcpy(data, str, dataLen);
		}

		PeoplezString::PeoplezString(char const * const __restrict__ str, size_t const len) noexcept(false) : dataLen(len), copies((COUNTER *) NEW(dataLen + COPIES_SIZE)), data(((char *)copies) + COPIES_SIZE), reservedBytes(dataLen)
		{
			*copies = 0;
			if(len) memcpy(data, str, len);
		}

		PeoplezString::PeoplezString(std::string const & __restrict__ str) noexcept(false) : dataLen(str.size()), copies((COUNTER *) NEW(dataLen + COPIES_SIZE)), data(((char*)copies) + COPIES_SIZE), reservedBytes(dataLen)
		{
			*copies = 0;
			memcpy(data, str.c_str(), dataLen);
		}

		PeoplezString::PeoplezString(PeoplezString const & __restrict__ other) noexcept : dataLen(other.dataLen), copies(other.copies), data(other.data), reservedBytes(other.reservedBytes)
		{
			++(*copies);
		}

		PeoplezString::PeoplezString(PeoplezString && other) noexcept : dataLen(other.dataLen), copies(other.copies), data(other.data), reservedBytes(other.reservedBytes)
		{
			other.copies = 0;
		}

		PeoplezString::PeoplezString(PeoplezString const & __restrict__ other, size_t from, size_t const len) noexcept : dataLen(other.Length()), copies(other.copies), data(other.data), reservedBytes(other.reservedBytes)
		{
			//Shift start position
			from = min(from, dataLen);
			data += from;

			//Set length
			dataLen = min(len, dataLen - from);

			++(*copies);
		}

		void PeoplezString::Append(PeoplezString const & str) noexcept(false)
		{
			if(!str.IsEmpty())
			{
				size_t const len = str.Length();
				ToUnique(dataLen + len);
				//Copying data
				memcpy(data + Length(), str.data, len);
				//Set new dataLength
				dataLen += len;
			}
		}

		void PeoplezString::Append(char const * const str) noexcept(false)
		{
			assert(str);

			Append(str, strlen(str));
		}

		void PeoplezString::Append(char const * const str, size_t const len) noexcept(false)
		{
			assert(str);

			if(len > 0)
			{
				ToUnique(Length() + len);
				//Copying data
				memmove(data + Length(), str, len);
				//Set new dataLength
				dataLen += len;
			}
		}

		void PeoplezString::Append(std::string & __restrict__ str) noexcept(false)
		{
			if(!str.empty())
			{
				ToUnique(Length() + str.length());
				//Copying data
				memcpy(data + Length(), str.c_str(), str.length());
				//Set new dataLength
				dataLen += str.length();
			}
		}

		bool PeoplezString::BeginsWith(char const *other, size_t const len) const noexcept
		{
			return len <= Length() && !memcmp(data, other, len);
		}

		void PeoplezString::Clear() noexcept
		{
			data = ((char *) copies) + COPIES_SIZE;
			dataLen = 0;
		}

		bool PeoplezString::EqualTo(size_t const pos, size_t const len, const PeoplezString & str) const noexcept
		{
			return Length() >= pos + len && str.Length() >= len && !memcmp(data + pos, str.data, len);
		}

		bool PeoplezString::EqualTo(size_t const pos, char const * const other, size_t const len) const noexcept
		{
			return Length() >= pos + len && !memcmp(data + pos, other, len);
		}

		int PeoplezString::Compare(PeoplezString const &other) const noexcept
		{
			size_t const minimum = min(Length(), other.Length());
			int const cmp = memcmp(other.data, data, minimum);

			return cmp ? cmp : (other.Length() < Length() ? -1 : (other.Length() > Length() ? 1 : 0));
		}

		void PeoplezString::Compress() noexcept(false)
		{
			size_t const dataLen = Length();
			unsigned int const stepSize = dataLen / 2 > 1024 ? dataLen / 2 : 1024;
			unsigned char *newCopies = (unsigned char *)CALLOC(stepSize + COPIES_SIZE, 1);
			unsigned char *buf = newCopies + COPIES_SIZE;
			unsigned int bufSize = stepSize;

			z_stream strm;
			strm.zalloc = 0;
			strm.zfree = 0;
			strm.next_in = (unsigned char *)data;
			strm.avail_in = dataLen;
			strm.next_out = buf;
			strm.avail_out = stepSize;

			deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, (16+15), 9, Z_DEFAULT_STRATEGY);

			while (strm.avail_in != 0)
			{
				deflate(&strm, Z_NO_FLUSH);

				if (!strm.avail_out)
				{
					bufSize += stepSize;
					newCopies = (unsigned char *) REALLOC(buf, bufSize + COPIES_SIZE);
					buf = newCopies + COPIES_SIZE;
					strm.next_out = buf + bufSize - stepSize;
					strm.avail_out = stepSize;
				}
			}

			int deflate_res = Z_OK;
			while (deflate_res == Z_OK)
			{
				if (!strm.avail_out)
				{
					bufSize += stepSize;
					newCopies = (unsigned char *) REALLOC(buf, bufSize + COPIES_SIZE);
					buf = newCopies + COPIES_SIZE;
					strm.next_out = buf + bufSize - stepSize;
					strm.avail_out = stepSize;
				}

				deflate_res = deflate(&strm, Z_FINISH);
			}

			deflateEnd(&strm);

			Reset((char *)newCopies, stepSize - strm.avail_out, bufSize);
		}

		PeoplezString& PeoplezString::EnsureZeroTermination() noexcept(false)
		{
			if(!Length() || *(data + Length() - 1) != '\0') Append(&zeroTermination, 1);
			return *this;
		}

		bool PeoplezString::EqualTo(char const * const other, size_t const len) const noexcept
		{
			assert(other);

			return len == Length() && !memcmp(data, other, Length());
		}

		size_t PeoplezString::Find(char const c, size_t const pos) const noexcept
		{
			if(pos < dataLen)
			{
				char const *result = (char const *) memchr(data + pos, c, dataLen - pos);

				return (result) ? result - data : NPOS;
			}

			return NPOS; //If the position is out of range
		}

		size_t PeoplezString::FindLast(char const c) const noexcept
		{
#ifdef __linux
			char const * const i = (char const * const) memrchr(data, c, Length());

			return i ? (int64_t) (i - data) : NPOS;

#else
			char const *i;
			for(i = data + Length() - 1; i >= data && *i != c; --i);

			return (int64_t) (i - data);
#endif
		}

		size_t PeoplezString::FindDoubleNewLine(size_t const startPos) const noexcept
		{
			size_t const len = Length();

			if(len > 3) [[likely]]
			{
				size_t const sLen = len - 3; //Search length
				char const *const end = data + sLen;

				for(char const *pos = (char const *) memchr(data + startPos, '\r', sLen); pos; pos = (char const *) memchr(pos + 1, '\r', end - pos))
				{
					if(pos[1] == '\n' && pos[2] == '\r' && pos[3] == '\n') return pos - data;
				}
			}

			return NPOS;
		}

		size_t PeoplezString::FindEndOfLine(size_t const startPos) const noexcept
		{
			if(startPos < dataLen && dataLen - startPos > 1)
			{
				char const * const end = data + dataLen - 1;
				char const * position = data + startPos;

				while(true)
				{
					position = (char const *) memchr(position, '\r', end - position);

					if(position)
					{
						if(position[1] == '\n') return position - data;
						else ++position;
					}
					else return NPOS;
				}
			}

			return NPOS;
		}

		/*
		size_t PeoplezString::FindNonSpace(size_t start = 0, size_t end = Length()) const
		{
			char const * posBeg = data + start;
			char const * posEnd = posBeg + (end < Length() ? end : Length());

			while(posBeg < posEnd && isspace(*posBeg)) ++posBeg;

			return posBeg - data;
		}
		*/

		size_t PeoplezString::HashValue() const noexcept
		{
			std::hash<char> hasher;
			size_t result = 0;

			for(size_t i = 0; i < dataLen; ++i)
			{
				result ^= hasher(data[i]) + 0x9e3779b9 + (result << 6) + (result >> 2);
			}

			return result;
		}

		bool PeoplezString::IsASCIICompatible(size_t const offset) const noexcept
		{
			unsigned char const * pos = (unsigned char *) data + offset;

			for(unsigned int rest = min((size_t)Alignment::alignmentRest(pos, 8), Length() - offset); rest; --rest, ++pos)
			{
				if(*pos & (unsigned char)0x80) return false;
			}
			{
				unsigned char const * const simd_end = (unsigned char const *) data + Length() - 7;
				uint64_t const simd_token = 0x8080808080808080;

				for(; pos < simd_end; pos += 8)
				{
					uint64_t const * const simd_pos = (uint64_t const *) pos;

					uint64_t const cmp = (*simd_pos & simd_token);

					if(cmp > 0) return false;
				}
			}
			for(unsigned char const * const end = (unsigned char const *) data + Length(); pos < end; ++pos)
			{
				if(*pos & (unsigned char)0x80) return false;
			}

			return true;
		}

		bool PeoplezString::IsUrlEncoded() const noexcept
		{
			unsigned char const *end = (unsigned char *) data + dataLen - 1;

			for(unsigned char const *start = (unsigned char *) data; start < end; ++start) if(*start != '%' && (*start < '0' || (*start > '9' && *start < 'A') || (*start > 'Z' && *start < 'a') || *start > 'z')) return false;

			return true;
		}

		void PeoplezString::Reset(char * const __restrict__ newData, size_t const newDataLen, size_t const newReserved)
		{
			// TODO Check wheter newData is right here (or newDataLen instead)
			if(newData)
			{
				if(Unique()) DELETE(copies);
				else --(*copies);

				copies = (COUNTER *)newData;
				data = newData + COPIES_SIZE;
				dataLen = newDataLen;
				reservedBytes = newReserved;
			}
			else Clear();
		}

		void PeoplezString::Resize(size_t const newSize) noexcept(false)
		{
			size_t const offSize = OffsetSize();

			if(Unique())
			{
				// move data to front
				if(offSize && newSize) memmove(((char *) copies) + COPIES_SIZE, data, min(Length(), newSize));

				// resize
				if(newSize != reservedBytes) copies = (COUNTER *) REALLOC(copies, newSize + COPIES_SIZE);

				// reset size infos
				data = ((char *) copies) + COPIES_SIZE;
				dataLen = min(reservedBytes = newSize, dataLen);
			}
			else
			{
				if(offSize || newSize != reservedBytes)
				{
					// create new char array
					--(*copies);
					copies = (COUNTER *)  NEW(newSize + COPIES_SIZE);
					(*copies) = 0;

					// reset size infos
					dataLen = min(newSize, dataLen);
					reservedBytes = newSize;

					// calculate beginning of new data
					char * const newData = ((char *)copies) + COPIES_SIZE;

					// copy data
					if(dataLen) memcpy(newData, data, dataLen);

					// set internal beginning of new data
					data = newData;
				}
			}
		}

		void PeoplezString::SetTo(char const * const str, size_t const len)
		{
			assert(str);

			// Reset fundamentals
			dataLen = len;
			data = ((char *) copies) + COPIES_SIZE;

			// if rhs contains data ...
			if(len)
			{
				// resize if neccessary
				if(reservedBytes < dataLen) Resize(dataLen);

				// ensure uniqueness
				ToUnique();

				// copy data
				memcpy(data, str, len);
			}
		}

		template<>
		void PeoplezString::Split<true>(std::vector<PeoplezString> & result, char const token) const
		{
			uint64_t const tokens = Count(token, 0) + 1;
			size_t const len = Length();
			char const * sourceChar = data; //Also used as "before"
			char const * const end = sourceChar + len;
			char const * position = sourceChar;

			// Clear result
			result.clear();
			if(result.capacity() < tokens) result.reserve(tokens);

			// Split string
			for(char const *pos = (char const *) memchr(sourceChar, token, len); pos; pos = (char const *) memchr(position, token, end - position))
			{
				sourceChar = position;
				position = pos + 1;

				if(sourceChar < pos) result.push_back(Substring(sourceChar - data, pos - sourceChar));
			}

			if(position < end)
			{
				result.push_back(Substring(position - data, end - position));
			}
		}

		template<>
		void PeoplezString::Split<false>(std::vector<PeoplezString> & result, char const token) const
		{
			uint64_t const tokens = Count(token, 0) + 1;
			size_t const len = Length();
			char const * sourceChar = data; //Also used as "before"
			char const * const end = sourceChar + len;
			char const * position = sourceChar;

			// Clear result
			result.clear();
			if(result.capacity() < tokens) result.reserve(tokens);

			// Split string
			for(char const *pos = (char const *) memchr(sourceChar, token, len); pos; pos = (char const *) memchr(position, token, end - position))
			{
				sourceChar = position;
				position = pos + 1;

				result.push_back(Substring(sourceChar - data, pos - sourceChar));
			}

			result.push_back(Substring(position - data, end - position));
		}

		void PeoplezString::Split(std::vector<PeoplezString> & result, char const token, bool const compress) const
		{
			compress ? Split<true>(result, token) : Split<false>(result, token);
		}

		void PeoplezString::SplitToPairs(std::vector<NameValuePair> & result, char const token1, char const token2, bool const compress) const
		{
			size_t numPairs = 0;
			char const * const end = data + Length();
			char const * pairPos = data;
			char const * pairEnd;

			//Count pairs
			do
			{
				//Search for next occurence of token1
				pairEnd = (char const *) memchr(pairPos, token1, end - pairPos);

				if(pairEnd) //if token1 found
				{
					if(pairEnd > pairPos || !compress) ++numPairs; //Must be added?

					//Set pairPos
					pairPos = pairEnd + 1;
				}
				else //if not more token1 found
				{
					if(pairPos < end || !compress) ++numPairs; //Must be added?
				}
			}
			while(pairEnd);

			//Clean up
			result.clear();
			result.reserve(numPairs);
			pairPos = data;

			//Fill result vector
			//numPairs := numPairsLeft
			while(numPairs)
			{
				//Search for next occurence of token1
				pairEnd = (char const *) memchr(pairPos, token1, end - pairPos);

				//If no more token1 found ...
				//Set pair end to end of string
				if(!pairEnd) pairEnd = end;

				if(pairEnd > pairPos) //Must be added?
				{
					//Search for token2 inside pair
					char const * const posToken2 = (char const * const) memchr(pairPos, token2, pairEnd - pairPos);

					if(posToken2) //if token2 found ...
					{
						//Add complete pair
						result.push_back(NameValuePair(Substring(pairPos - data, posToken2 - pairPos), Substring(posToken2 + 1 - data, pairEnd - posToken2 - 1)));
					}
					else
					{
						//Add with empty value
						result.push_back(NameValuePair(Substring(pairPos - data, pairEnd - pairPos), PeoplezString()));
					}

					//1 less pair left
					--numPairs;
				}
				else if(!compress)
				{
					//Add empty pair
					result.push_back(NameValuePair());

					//Also 1 less pair left
					--numPairs;
				}

				if(pairEnd) //if token1 found
				{


					//Set pairPos
					pairPos = pairEnd + 1;
				}
				else //if not more token1 found
				{
					if(pairPos < end || !compress) ++numPairs; //Must be added?
				}
			}
		}

		int64_t PeoplezString::ToInt64(unsigned char const base) const noexcept(false)
		{
			return Parsing::ToInt<int64_t>(data, Length(), base);
		}

		uint64_t PeoplezString::ToUInt64(unsigned char const base) const noexcept(false)
		{
			return Parsing::ToInt<uint64_t>(data, Length(), base);
		}

		void PeoplezString::ToLower() noexcept(false)
		{
			ToUnique();
			std::transform(data, data + dataLen, data, ::tolower);
		}

		void PeoplezString::ToLower_ASCII_NU() noexcept
		{
			char const * const end = data + Length();

			for(char * pos = data; pos < end; ++pos)
			{
				if(*pos >= 'A' && *pos <= 'Z') *pos += 32;
			}
		}

		void PeoplezString::ToLower_ASCII() noexcept(false)
		{
			ToUnique();
			ToLower_ASCII_NU();
		}

		void PeoplezString::ToUnique(size_t const newSize) noexcept(false)
		{
			size_t const offSize = OffsetSize();

			if(*copies || (newSize && offSize > newSize / 2))
			{
				--(*copies);
				copies = (COUNTER *) NEW(newSize + COPIES_SIZE);
				*copies = 0;

				dataLen = min(newSize, dataLen);

				char * const newData = ((char *)copies) + COPIES_SIZE;
				if(dataLen) memcpy(newData, data, dataLen);
				data = newData;

				reservedBytes = newSize;
			}
			else if(reservedBytes - offSize < newSize)
			{
				reservedBytes = newSize + offSize;
				dataLen = min(newSize, dataLen);

				copies = (COUNTER *) REALLOC(copies, reservedBytes + COPIES_SIZE);
				data = ((char *) copies) + COPIES_SIZE;
			}
			//else nothing is to do
		}

		void PeoplezString::ToUpper() noexcept(false)
		{
			ToUnique();
			std::transform(data, data + dataLen, data, ::toupper);
		}

		void PeoplezString::ToUpper_ASCII_NU() noexcept
		{
			char const * const end = data + Length();

			for(char * pos = data; pos < end; ++pos)
			{
				if(*pos >= 'a' && *pos <= 'z') *pos -= 32;
			}
		}

		void PeoplezString::ToUpper_ASCII() noexcept(false)
		{
			ToUnique();
			ToUpper_ASCII_NU();
		}

		void PeoplezString::Trim() noexcept
		{
			char const * end = data + Length();

			// Find beginning
			for(; data < end && (*data == ' ' || *data == '\r' || *data == '\n' || *data == '\t'); ++data);

			// Find end
			for(--end; end >= data && (*end == ' ' || *end == '\r' || *end == '\n' || *end == '\t'); --end);
			++end;

			dataLen = end - data;
		}

		void PeoplezString::TrimFast() noexcept
		{
			char const * end = data + Length();

			// Find beginning
			for(; data < end && *data <= ' '; ++data);

			// Find end
			for(--end; end >= data && *end <= ' '; --end);
			++end;

			dataLen = end - data;
		}

		bool PeoplezString::DecodeUrl() noexcept(false)
		{
			int escapes = 0;
			unsigned char const *sourcePos = (unsigned char *) data;
			unsigned char const *end = (unsigned char *) data + Length();

			for(; sourcePos < end; ++sourcePos)
			{
				if(*sourcePos == '%')
				{
					//Check if escape sequence can be completely inside the text; Check if the following 2 characters are HEX characters
					if(sourcePos > end - 3 || !HEX2DEC[*(sourcePos + 1)] || !HEX2DEC[*(sourcePos + 2)] ) return false; //RETURN!!!

					++escapes;
				}
			}

			if(escapes)
			{
				uint32_t targetLen = Length() - 2 * escapes;

				ToUnique();

				char *targetPos = data;

				for(sourcePos = (unsigned char *) data, end = sourcePos + Length(); sourcePos < end; ++sourcePos)
				{
					if(*sourcePos == '%')
					{
						++sourcePos;
						*targetPos = (*sourcePos) - HEX2DEC[*sourcePos];

						*targetPos <<= 4;

						++sourcePos;
						*targetPos |= (*sourcePos) - HEX2DEC[*sourcePos];
					}
					else if(*sourcePos == '+') *targetPos = ' ';
					else *targetPos = *sourcePos;

					++targetPos;
				}

				dataLen = targetLen;
			}
			return true;
		}

		bool PeoplezString::EncodeHtml() noexcept(false)
		{
			unsigned char const * const srcEnd = (unsigned char *) data + dataLen;
			size_t const targetReserved = dataLen * 6;
			uint64_t *const targetCopies = (uint64_t *) NEW(targetReserved + COPIES_SIZE);
			unsigned char *const targetStart = ((unsigned char *)targetCopies) + COPIES_SIZE;
			unsigned char *targetPos = targetStart;
			size_t charNum;
			unsigned char buffer[7];
			size_t bufferSize = 0;
			unsigned char a, b, c ,d;

			for(unsigned char *srcPos = (unsigned char *) data; srcPos < srcEnd; ++srcPos)
			{
				size_t const leadingOnes = std::countl_one(*srcPos);

				//Ermitteln der Zeichennummer
				switch(leadingOnes)
				{
				case 0:
					a = *srcPos;
					charNum = (unsigned int) a;
					break;
				case 2:
					a = *srcPos++;
					b = *srcPos;
					charNum = ((a & 0x1F) << 6) | (b & 0x3F);
					break;
				case 3:
					a = *srcPos++;
					b = *srcPos++;
					c = *srcPos;
					charNum = ((((a & 0xF) << 6) | (b & 0x3F)) << 6) | (c & 0x3F);
					break;
				case 4:
					a = *srcPos++;
					b = *srcPos++;
					c = *srcPos++;
					d = *srcPos;
					charNum = ((((((a & 0x07) << 6) | (b & 0x3F)) << 6) | (c & 0x3F)) << 6) | (d & 0x3F);
					break;
				default:
					DELETE(targetCopies);
					return false;
				}

				if(charNum > 31 && charNum < 127 && charNum != 34 && charNum != 38 && charNum != 60 && charNum != 62) //Falls das zeichen nicht escapet werden muss ...
				{
					*targetPos++ = *srcPos;
				}
				else //Falls das Zeichen escaped werden muss ...
				{
					if(!charNum) //Falls das Zeichen die Nummer 0 hat
					{
						buffer[0] = '0';
						bufferSize = 1;
					}
					else //Sonst ...
					{
						for(bufferSize = 0; charNum > 0; ++bufferSize, charNum /= 10) buffer[bufferSize] = (charNum % 10) + 48;
					}

					//Schreiben des Buffers in die Ausgabe
					*targetPos++ = '&';
					*targetPos++ = '#';
					while(bufferSize > 0)
					{
						--bufferSize;
						*targetPos++ = buffer[bufferSize];
					}
					*targetPos++ = ';';
				}
			}

			Clear();
			ToUnique();

			DELETE(copies);
			copies = targetCopies;
			*copies = 0;
			data = (char *) targetStart;
			dataLen = targetPos - targetStart;
			reservedBytes = targetReserved;
			return true;
		}

		void PeoplezString::EncodeUrl() noexcept(false)
		{
			int escapes = 0;
			unsigned char const * const end = (unsigned char *) data + Length();

			for(unsigned char const *start = (unsigned char *) data; start < end; ++start) if(GetUrlEncMapPos(*start)) ++escapes;

			if(escapes)
			{
				uint32_t const outputLength = Length() + 2 * escapes;
				uint64_t *const outputCopies = (uint64_t *) NEW(COPIES_SIZE + outputLength);
				char *const output = ((char *)outputCopies) + COPIES_SIZE;
				unsigned char *toPos = (unsigned char *) output;

				for(unsigned char const *start = (unsigned char *) data; start < end; ++start)
				{
					if(*start == ' ') *toPos++ = '+';
					if(GetUrlEncMapPos(*start))
					{
						*toPos++ = '%';
						*toPos++ = DEC2HEX[(*start >> 4) & 0x0F];
						*toPos++ = DEC2HEX[*start & 0x0F];
					}
					else *toPos++ = *start;
				}

				Clear();
				ToUnique();

				DELETE(copies);
				copies = outputCopies;
				*copies = 0;
				data = output;
				reservedBytes = dataLen = outputLength;
			}
		}

		PeoplezString & PeoplezString::operator =(PeoplezString const & rhs) noexcept
		{
			if(Unique()) DELETE(copies);
			else --(*copies);

			data = rhs.data;
			dataLen = rhs.dataLen;
			reservedBytes = rhs.reservedBytes;
			copies = rhs.copies;
			++(*copies);

			return *this;
		}

		PeoplezString & PeoplezString::operator =(PeoplezString && rhs) noexcept
		{
			if(Unique()) DELETE(copies);
			else --(*copies);

			dataLen = rhs.dataLen;
			copies = rhs.copies;
			data = rhs.data;
			reservedBytes = rhs.reservedBytes;
			rhs.copies = 0;

			return *this;
		}

		PeoplezString & PeoplezString::operator =(const std::string & __restrict__ rhs) noexcept(false)
		{
			// determine new data length
			dataLen = rhs.size();

			// reset beginning of data
			data = ((char *) copies) + COPIES_SIZE;

			// if rhs contains data ...
			if(dataLen)
			{
				// resize if neccessary
				if(reservedBytes < dataLen) Resize(dataLen);

				// ensure uniqueness
				ToUnique();

				// copy data
				memcpy(data, rhs.c_str(), dataLen);
			}

			return *this;
		}

		PeoplezString & PeoplezString::operator =(char const * const rhs) noexcept(false)
		{
			// determine new data length
			dataLen = strlen(rhs);

			// reset beginning of data
			data = ((char *) copies) + COPIES_SIZE;

			// if rhs contains data ...
			if(dataLen)
			{
				// resize if neccessary
				if(reservedBytes < dataLen) Resize(dataLen);

				// ensure uniqueness
				ToUnique();

				// copy data
				memcpy(data, rhs, dataLen);
			}

			return *this;
		}

		PeoplezString & PeoplezString::operator +=(const PeoplezString & rhs) noexcept(false)
		{
			Append(rhs);
			return *this;
		}

		PeoplezString & PeoplezString::operator +=(char const * rhs) noexcept(false)
		{
			Append(rhs);
			return *this;
		}

		PeoplezString PeoplezString::operator +(PeoplezString const & rhs) const noexcept(false)
		{
			PeoplezString result = *this;
			return result += rhs;
		}

		PeoplezString PeoplezString::operator +(char const * rhs) const noexcept(false)
		{
			PeoplezString result = *this;
			return result += rhs;
		}

		bool PeoplezString::operator ==(const PeoplezString & rhs) const noexcept
		{
			return rhs.Length() == Length() && !memcmp(data, rhs.data, Length());
		}

		bool PeoplezString::operator ==(char const * const rhs) const noexcept
		{
			size_t const len = strlen(rhs);
			return len == Length() && !memcmp(data, rhs, len);
		}

		bool PeoplezString::operator !=(const PeoplezString & rhs) const noexcept
		{
			return Length() != rhs.Length() || !std::equal(data, data + Length(), rhs.data);
		}

		bool PeoplezString::operator <(PeoplezString const & rhs) const noexcept
		{
			size_t const minimum = min(Length(), rhs.Length());
			int const cmp = memcmp(data, rhs.data, minimum);

			if(!cmp) return Length() < rhs.Length();
			else return cmp < 0;
		}

		bool PeoplezString::operator >(PeoplezString const & rhs) const noexcept
		{
			size_t const minimum = min(Length(), rhs.Length());
			int const cmp = memcmp(data, rhs.data, minimum);

			if(!cmp) return Length() > minimum;
			else return cmp > 0;
		}

		PeoplezString PeoplezString::ParseDec(uint64_t input) noexcept(false)
		{
			char outputStart[20];
			char *outputPos = outputStart + 20;

			do *(--outputPos) = DEC2HEX[input % 10]; while(input /= 10);

			return PeoplezString(outputPos, 20 + outputStart - outputPos);
		}

		PeoplezString PeoplezString::ParseHex(uint64_t input) noexcept(false)
		{
			char outputStart[16];
			char *outputPos = outputStart + 16;

			do *(--outputPos) = DEC2HEX[input & 0xf]; while(input >>= 4);

			return PeoplezString(outputPos, 16 + outputStart - outputPos);
		}

		void PeoplezString::Write(FILE * const __restrict__ stream, size_t const offset, bool const zeroTerminated) const
		{
			assert(stream);

			fwrite(data + offset, sizeof(char), Length() - offset, stream);
			if(zeroTerminated) fwrite(&zeroTermination, sizeof(char), 1, stream);
		}

		PeoplezString::~PeoplezString() noexcept
		{
			if(copies)
			{
				if(Unique()) DELETE(copies);
				else --(*copies);
			}
		}

		ostream& operator<<(ostream & __restrict__ os, PeoplezString const & __restrict__ dt) noexcept(false)
		{
			os.write(dt.GetData(), dt.Length());
			return os;
		}
	} // namespace String
} // namespace Peoplez
