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

#ifndef PEOPLEZ_STRING_PEOPLEZSTRING_H_
#define PEOPLEZ_STRING_PEOPLEZSTRING_H_

// Internal includes
#include "../String/Parsing/StringToInt.hpp"

// External includes
#include <boost/unordered_map.hpp>
#include <cstdint>
#include <vector>
#include <stdexcept>

#ifndef PRIVATE
	#define PRIVATE private
#endif

namespace Peoplez
{
	namespace String
	{
		/**
		 * @typedef uint64_t COUNTER
		 * @brief Definition of type for copies counter
		 */
		typedef uint64_t COUNTER;

		class PeoplezString;
		/**
		 * @var typedef pair<PeoplezString, PeoplezString> NameValuePair
		 * @brief Pair of two PeoplezStrings
		 */
		typedef std::pair<PeoplezString, PeoplezString> NameValuePair;

		/**
		 * @brief Container for text (and other char array data)
		 * @details The class is the peoplez specific container for text and binary data. It cares of deleting the array itself. Doesn't copy the data!
		 *
		 * @par Data races
		 * The whole class is not thread safe
		 */
		class PeoplezString final
		{
		public:
			/**
			 * Standard constructor
			 *
			 * @param reservation Number of bytes to reserve for future data
			 *
			 * @par Exception Safety
			 *  Strong exception safety
			 */
			explicit PeoplezString(size_t reservation = 0) noexcept(false);
			/**
			 * Special constructor
			 * @param str '\0'-terminated C-string
			 *
			 * @par Exception Safety
			 *  Strong exception safety
			 */
			PeoplezString(char const * str) noexcept(false);
			/**
			 * Special constructor
			 *
			 * @param str char array with data
			 * @param len array length (without '\0'-termination)
			 *
			 * @par Exception Safety
			 *  Strong exception safety
			 */
			explicit PeoplezString(char const * str, size_t len) noexcept(false);
			/**
			 * Special constructor
			 *
			 * @param str Original string
			 *
			 * @par Exception Safety
			 *  Strong exception safety
			 */
			PeoplezString(std::string const & str) noexcept(false);
			/**
			 * Copy constructor
			 *
			 * @param other The string to copy from
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			PeoplezString(PeoplezString const & other) noexcept;
			/**
			 * Move constructor
			 *
			 * @param other The original string to move
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			PeoplezString(PeoplezString && other) noexcept;
			/**
			 * Destructor
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			~PeoplezString() noexcept;

			/**
			 * Appends a PeoplezString to this PeoplezString
			 *
			 * @param str PeoplezString that should get appended
			 */
			void Append(PeoplezString const & str) noexcept(false);
			/**
			 * Appends a char array with to this PeoplezString
			 *
			 * @param str char array with the data to be appended
			 */
			void Append(char const * str) noexcept(false) __attribute__((nonnull));
			/**
			 * Appends a char array with the given length to this PeoplezString
			 *
			 * @param str char array with the data to be appended
			 * @param len Length of the data to get appended in bytes (without '\0' termination)
			 */
			void Append(char const * str , size_t len) noexcept(false) __attribute__((nonnull));
			/**
			 * Appends a std::string to this PeoplezString
			 *
			 * @param str String that should get appended
			 */
			void Append(std::string & str) noexcept(false);
			/**
			 * Checks whether this string begins with the given one
			 *
			 * @param other The string that this should begin with
			 *
			 * @return True: this string begins with other; False: otherwise
			 */
			inline bool BeginsWith(PeoplezString const & other) const noexcept {return BeginsWith(other.data, other.Length());}
			/**
			 * Checks whether this string begins with the given char array
			 *
			 * @param other The char array that this string should begin with
			 * @param len Length of the char array other
			 *
			 * @return True: this string begins with the char array; False: otherwise
			 */
			bool BeginsWith(char const * other, size_t len) const noexcept __attribute__((pure));
			/**
			 * Ensures that the string is empty
			 */
			void Clear() noexcept;
			/**
			 * Compares this string with the other
			 *
			 * @param other Other string to compare with
			 *
			 * @return <0: other < this; 0: equal; >0: other > this
			 */
			int Compare(PeoplezString const &other) const noexcept __attribute__((pure));
			/**
			 * Compresses the string in gzip format
			 *
			 * Recommended for text only
			 */
			void Compress() noexcept(false);
			/**
			 * Counts the number of occurrences of a given token
			 *
			 * @param token Token to count
			 * @param offset Position to begin searching
			 *
			 * @return Number of occurrences of the token
			 */
			size_t Count(char token, size_t offset = 0) const noexcept __attribute__((pure));
			size_t Count2(char token, size_t offset = 0) const noexcept __attribute__((pure));
			size_t Count3(char token, size_t offset = 0) const noexcept __attribute__((pure));
			/**
			 * Ensures that the sting has a terminating '\0'
			 *
			 * @return This string itself
			 */
			PeoplezString& EnsureZeroTermination() noexcept(false);
			/**
			 * Compares this string to another one
			 *
			 * @param pos Position of the first character to compare in this string
			 * @param len Number of characters to be compared
			 * @param str The other string to compare with
			 *
			 * @return True: Equal; False: Not equal
			 */
			bool EqualTo(size_t pos, size_t len, const PeoplezString &str) const noexcept __attribute__((pure));
			/**
			 * Compares this string to a char array
			 *
			 * @param pos Intex of the first character to compare in this string
			 * @param other The char array to compare with
			 * @param len Number of characters to be compared
			 *
			 * @return True: Equal; False: Not equal
			 */
			bool EqualTo(size_t pos, char const * other, size_t len) const noexcept __attribute__((pure));
			/**
			 * Compares this string to a char array
			 *
			 * @param other The char array to compare with
			 * @param len Number of characters to be compared
			 */
			bool EqualTo(char const * other, size_t len) const noexcept __attribute__((nonnull, pure));
			/**
			 * Searches the first occurrance of the given character
			 *
			 * @param c Character to search for
			 * @param pos Start position where to begin to search
			 *
			 * @return index of the first occurrance of the character; NPOS if not found
			 */
			size_t Find(char c, size_t pos = 0) const noexcept __attribute__((pure));
			/**
			 * Searches the last occurrance of the given character
			 *
			 * @param c Character to search for
			 *
			 * @return index of the last occurrance of the character; NPOS if not found
			 */
			size_t FindLast(char c) const noexcept __attribute__((pure));
			/**
			 * Searches the first appearance of a double line break
			 *
			 * @param startPos The position of the first character of the substring to search in
			 *
			 * @return Position of the first character of the double line break or NPOS if not found. (For "123\r\n\r\n89" the return value would be 3 + startPos)
			 */
			size_t FindDoubleNewLine(size_t startPos = 0) const noexcept __attribute__((pure));
			/**
			 * Searches the end of the first line in the given text part
			 *
			 * @param startPos Beginning of the text to search in (Beginning of the text part, not of the whole text)
			 *
			 * @return Position of the "\r\n" sequence (For "123\r\n6789": 3 + startPos)
			 */
			size_t FindEndOfLine(size_t startPos = 0) const noexcept __attribute__((pure));
			//size_t FindNonSpace(size_t start = 0, size_t end = Length()) const;
			/**
			 * Getter for the data

			 * @return Shared array containing the data WITHOUT zero termination
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			inline char const *GetData() const noexcept {return data;}
			/**
			 * Calculates a hash value of this string
			 *
			 * @return Hash value of this string
			 */
			inline size_t HashValue() const noexcept {return Length() > 0 ? boost::hash_range(GetData(), GetData() + Length()) : 0;}
			bool IsASCIICompatible(size_t offset) const noexcept;
			/**
			 * Checks whether the string is empty
			 *
			 * @return True if string is empty, false otherwise
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			inline bool IsEmpty() const noexcept {return !Length();}
			/**
			 * Checks whether this string is already url encoded
			 *
			 * @return True: Already url encoded; False: Not yet completely url encoded
			 */
			bool IsUrlEncoded() const noexcept __attribute__((pure));
			/**
			 * Indicated whether the data are '\0' terminated
			 *
			 * @return true: Is '\0' terminated; false: Is not '\0' terminated
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			bool IsZeroTerminated() const noexcept {return (dataLen <= 0 ? false : (data[dataLen - 1] == '\0'));}
			/**
			 * Getter for the length
			 *
			 * Length of the text in text mode or length of the data in bytes (without '\0' termination)
			 *
			 * @return Length of the text or the data
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			inline size_t Length() const noexcept {return dataLen;}
			/**
			 * Ensures that exactly the given size is reserved and kills the offset
			 *
			 * @param newSize Size of the new allocated memory in bytes
			 *
			 * @return true: Reallocation successful; false: Reallocation failed
			 */
			void Resize(size_t newSize) noexcept(false);
			/**
			 * Multi argument version of operator=
			 * Has the same effect as
			 *
			 * @param str Original CString
			 * @param len Length of string
			 */
			void SetTo(char const * const str, size_t const len);
			/**
			 * Template version of Split
			 * Splits the string at every occurence of the given token
			 * Faster than parameter only version
			 *
			 * Works byte wise. Works well with ASCII encoded text, ISO/IEC 8859-x encoded text and ASCII chars in UTF-8 encoded text
			 *
			 * @param result Resulting list of text parts
			 * @param token Character at which the text has to get splitted
			 */
			template<bool COMPRESS>
			void Split(std::vector<PeoplezString> &result, char token) const;
			/**
			 * Splits the string at every occurrence of the given token
			 *
			 * @param result Resulting list of text parts
			 * @param token Character at which the text has to get splitted
			 * @param compress Indicates whether empty strings have to get removed
			 */
			void Split(std::vector<PeoplezString> &result, char token, bool compress) const;
			/**
			 * Splits the text into name value pairs
			 *
			 * Works byte wise. Works well with ASCII encoded text, ISO/IEC 8859-x encoded text and ASCII chars in UTF-8 encoded text
			 *
			 * @param result Resulting list of name value pairs
			 * @param token1 Character as seperator between name value pairs
			 * @param token2 Character as seperator between name and value of the same pair
			 * @param compress Indicates whether empty pairs have to be removed
			 */
			void SplitToPairs(std::vector<NameValuePair> & result, char token1, char token2, bool compress) const;
			template <class T>
			void SplitToPairs(std::vector<T> & result, PeoplezString (T::*mPtr1), PeoplezString (T::*mPtr2), char token1, char token2, bool compress) const
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
							T obj;
							obj.*mPtr1 = Substring(pairPos - data, posToken2 - pairPos);
							obj.*mPtr2 = Substring(posToken2 + 1 - data, pairEnd - posToken2 - 1);
							//result.push_back(NameValuePair(Substring(pairPos - data, posToken2 - pairPos), Substring(posToken2 + 1 - data, pairEnd - posToken2 - 1)));
							result.push_back(obj);
						}
						else
						{
							//Add with empty value
							T obj;
							obj.*mPtr1 = Substring(pairPos - data, pairEnd - pairPos);
							//result.push_back(NameValuePair(Substring(pairPos - data, pairEnd - pairPos), PeoplezString()));
							result.push_back(obj);
						}

						//1 less pair left
						--numPairs;
					}
					else if(!compress)
					{
						//Add empty pair
						//result.push_back(NameValuePair());
						result.push_back(T());

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
			/**
			 * Creates a substring
			 *
			 * Copies the data biginning at the given position.
			 * Boundary check is included. If startPos > Length() then startPos = Length().
			 *
			 * @param startPos Position of the first character to be copied
			 *
			 * @return Substring from startPos to end
			 */
			inline PeoplezString Substring(size_t const startPos) const {return PeoplezString(*this, startPos, Length() - startPos);}
			/**
			 * Creates a substring
			 *
			 * Copies the data beginning at the given position until position startPos + len.
			 * Boundary check is included. If startPos > Length() then startPos = Length(). If startPos + len > Length then len = Length - startpos.
			 *
			 * @param startPos Position of the first character to be copied
			 * @param len Number of characters to be copied
			 *
			 * @return Substring of length len beginning at startPos
			 */
			inline PeoplezString Substring(size_t const startPos, unsigned int const len) const {return PeoplezString(*this, startPos, len);}
			template<typename T> T ToInt(unsigned char base) const {return Parsing::ToInt<T>(data, Length(), base);}
			/**
			 * Converts this string to an int64_t
			 *
			 * @return Representation as in64_t if possible, 0 otherwise
			 */
			int64_t ToInt64(unsigned char base) const noexcept(false);
			/**
			 * Converts this string to an uint64_t
			 *
			 * @return Representation as uint64_t if possible, 0 otherwise
			 */
			uint64_t ToUInt64(unsigned char base) const noexcept(false);
			/**
			 * Transforms uppercase letters to lowercase
			 */
			void ToLower() noexcept(false);
			/**
			 * Transforms uppercase letters in ASCII to lowercase
			 *
			 * Transforms only ASCII letters in uppercase to lowercase.
			 * Ignores non ASCII letters but is faster than ToLower()
			 */
			void ToLower_ASCII() noexcept(false);
			/**
			 * Transforms uppercase letters in ASCII to lowercase. USE CAREFULLY!!!
			 *
			 * Like ToLower_ASCII() but does not ensure that the transformed data are not shared with other strings!
			 * This way all other copie that use the same data get the same transformations!
			 * That is also the fact for const copies and copies in other threads!
			 * This effect can be useful for splitted strings where no use case is left for the original string.
			 * In such cases the substrings can be modified using ..._NU() methods for better performance.
			 */
			void ToLower_ASCII_NU() noexcept;
			/**
			 * Ensures that this instance is unique
			 *
			 * Ensures that this instance doesn't share its data with other strings
			 */
			inline void ToUnique() noexcept(false) {ToUnique(Length());}
			/**
			 * Ensures that this instance is unique
			 *
			 * Ensures that this instance doesn't share its data with other strings and has at least the size of newSize reserved beginning at data ptr
			 *
			 * @param newSize Size of the reserved memory (without '\0' termination)
			 */
			void ToUnique(size_t newSize) noexcept(false);
			/**
			 * Transforms lowercase letters to uppercase
			 */
			void ToUpper() noexcept(false);
			/**
			 * Transforms lowercase letters in ASCII to uppercase
			 *
			 * Transforms only ASCII letters in lowercase to uppercase.
			 * Ignores non ASCII letters but is faster than ToUpper()
			 */
			void ToUpper_ASCII() noexcept(false);
			/**
			 * Transforms lowercase letters in ASCII to uppercase. USE CAREFULLY!!!
			 *
			 * Like ToUpper_ASII() but does not ensure that the transformed data are not shared with other strings!
			 * This way all other copies that use the same data get the same transformations!
			 * That is also the fact for const copies and copies in other threads!
			 * This effect can be useful for splitting strings where no use case is left for the original string.
			 * In such cases the substring can be modified using ..._NU() methods for better performance.
			 */
			void ToUpper_ASCII_NU() noexcept;
			/**
			 * Removes space characters from beginning and end of string
			 *
			 * The data of the string don't get changed, only the start and end pointers get relocated
			 */
			void Trim() noexcept;
			/**
			 * Removes all characters <= ' ' (32; 0x20) from beginning and end of string
			 *
			 * The data of the string don't get changed, only the start and end pointers get relocated
			 */
			void TrimFast() noexcept;
			/**
			 * Indicates whether this string is unique
			 *
			 * Indicates wheter this object shares its data with other strings
			 *
			 * @return true: Does not share, false: Shares data
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			inline bool Unique() const noexcept {return !(*copies);}
			/**
			 * Creates a unique copy
			 *
			 * Fast creation of a unique copy without unneccessary overhead
			 *
			 * @return Copy of the string with result.Unique() == true
			 */
			inline PeoplezString UniqueCopy() const {return PeoplezString(GetData(), Length());}
			/**
			 * Decodes url encoded data
			 */
			bool DecodeUrl() noexcept(false);
			/**
			 * Encodes data to html encoded data
			 */
			bool EncodeHtml() noexcept(false);
			/**
			 * Encodes data to url encoded data
			 */
			void EncodeUrl() noexcept(false);
			/**
			 * Copy assignment operator
			 *
			 * @param rhs Original PeoplezString object
			 *
			 * @return Reference to this object
			 */
			PeoplezString & operator =(PeoplezString const & rhs) noexcept;
			/**
			 * Move assignment operator
			 *
			 * @param rhs Original PeoplezString object
			 *
			 * @return Reference to this object
			 */
			PeoplezString & operator =(PeoplezString && rhs) noexcept;
			/**
			 * Assignment operator from std::string to PeoplezString
			 *
			 * @param rhs Original string
			 *
			 * return Reference to this object
			 */
			PeoplezString & operator =(std::string const & rhs) noexcept(false);
			/**
			 * Assignment operator for CString
			 *
			 * @param rhs Original CString
			 *
			 * return Reference to this object
			 */
			PeoplezString & operator =(char const * rhs) noexcept(false);
			/**
			 * Concatenation assignment operator
			 *
			 * @param rhs String to be appended
			 *
			 * @return Reference to this object
			 */
			PeoplezString & operator +=(PeoplezString const & rhs) noexcept(false);
			/**
			 * Concatenation assignment operator
			 *
			 * @param rhs CString to be appended
			 *
			 * @return Reference to this object
			 */
			PeoplezString & operator +=(char const * rhs) noexcept(false);
			/**
			 * Concatenation operator
			 *
			 * @param rhs String to be appended
			 *
			 * @return Concatenation of this string and rhs
			 */
			PeoplezString operator +(PeoplezString const & rhs) const noexcept(false);
			/**
			 * Concatenation operator
			 *
			 * @param rhs CString to be appended
			 *
			 * @return Concatenation of this string and rhs
			 */
			PeoplezString operator +(char const * rhs) const noexcept(false);
			/**
			 * Equation operator
			 *
			 * @param rhs String to compare with
			 *
			 * @return True if equal, false otherwise
			 */
			bool operator ==(PeoplezString const & rhs) const noexcept __attribute__((pure));
			/**
			 * Equation operator
			 *
			 * @param rhs CString to compare with
			 *
			 * @return True if equal, false otherwise
			 */
			bool operator ==(char const * rhs) const noexcept __attribute__((pure));
			/**
			 * Unequation operator
			 *
			 * @param rhs String to compare with
			 *
			 * @return False if equal, true otherwise
			 */
			bool operator !=(PeoplezString const & rhs) const noexcept __attribute__((pure));
			/**
			 * Indicates whether the first different character of this string has a lower ASCII value than the one in the given string or this string is the shorter one
			 *
			 * Compares the complete data part (including the \0)
			 *
			 * @param rhs String to compare with
			 *
			 * @return True: First different character in this string has a smaller value than in the other string; False: Otherwise or binary mode
			 */
			bool operator <(PeoplezString const & rhs) const noexcept __attribute__((pure));
			/**
			 * Creates a substring where rhs bytes are cut off the front.
			 * Does not cut more than Length() bytes
			 *
			 * @param rhs Number of bytes to cut from the front
			 *
			 * @return (lazy) Copy of this string with rhs bytes cut off the front
			 */
			PeoplezString operator <<(size_t rhs) {return Substring(rhs);}
			/**
			 * Creates a substring where rhs bytes are cut off the end
			 * Does not cut more than Length() bytes
			 *
			 * @param rhs Number of bytes to cut from the end
			 *
			 * @return (lazy) Copy of this string whith rhs bytes cut off the end
			 */
			PeoplezString operator >>(size_t rhs) {return Substring(0, rhs <= Length() ? Length() - rhs : 0);}
			/**
			 * Cuts rhs bytes from the front of this string
			 * Does not cut more than Length() bytes
			 *
			 * @param rhs Number of bytes to cut from the front
			 *
			 * @return Reference to this string
			 */
			PeoplezString & operator <<=(size_t rhs) noexcept {rhs = std::min(rhs, Length()); data += rhs; return *this;}
			/**
			 * Cuts rhs bytes from the end of this string
			 * Does not cut more than Length() bytes
			 *
			 * @param rhs Number of bytes to cut from the end
			 *
			 * @return Reference to this string
			 */
			PeoplezString & operator >>=(size_t rhs) noexcept {rhs = std::min(rhs, Length()); dataLen -= rhs; return *this;}
			/**
			 * Indicates whether the first different character of this string has a higher ASCII value than the one in the given string or this string is the longer one
			 *
			 * Compares the complete data part (including the \0)
			 *
			 * @param rhs String to compare with
			 *
			 * @return True: First different character in this string has a higher value than in the other string; False: Otherwise or binary mode
			 */
			bool operator >(PeoplezString const & rhs) const noexcept __attribute__((pure));
			/**
			 * Array subscript operator
			 *
			 * @param index Positon of the character
			 *
			 * @return char value at the given index
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			inline char operator[](size_t index) const noexcept {return data[index];}

			/**
			 * Array subscript operator
			 * Attention! This method does not ensure uniqueness!
			 *
			 * @param index Position of character
			 *
			 * @return reference to char value at given index
			 *
			 * @par Exception Safety
			 *  No-throw guarantee
			 */
			inline char & operator[](size_t index) noexcept {return data[index];}

			/**
			 * Parses a decimal unsigned number to a PeoplezString
			 *
			 * @param input Number to be parsed
			 *
			 * @return Given number as string
			 */
			static PeoplezString ParseDec(uint64_t input) noexcept(false);
			/**
			 * Parses a hexadecimal unsigned number to a PeoplezString
			 *
			 * @param input Number to be parsed
			 *
			 * @return Given number as string
			 */
			static PeoplezString ParseHex(uint64_t input) noexcept(false);

			/**
			 * Writes the string to the stream
			 *
			 * @param stream Stream to write to
			 * @param offset Index of first character to write
			 * @param zeroTerminated Indicates wheter the written string should be '\0' terminated
			 */
			void Write(FILE * stream, size_t offset, bool zeroTerminated = false);

			/**
			 * @brief Value for "no position"
			 */
			static const size_t NPOS;
		PRIVATE:
			/**
			 * Substing constructor
			 *
			 * Constructor for a substring - The substring is cut at the end of the original string if too long or behind its end
			 *
			 * @param other Original string
			 * @param from Beginning of substring relative to beginning of the original string
			 * @param len Length of the substring
			 */
			PeoplezString(PeoplezString const & other, size_t from, size_t len) noexcept;

			inline size_t OffsetSize() const noexcept {return data - (char *) copies - sizeof(COUNTER);}
			void Reset(char * newData, size_t newDataLen, size_t newReserved);

			static unsigned char const DEC2HEX[16];
			static unsigned char const HEX2DEC[256];
			/**
			 * @brief Length of the data
			 * @details Length of the data in bytes (within '\0' termination)
			 */
			size_t dataLen;
			COUNTER * copies;
			char * data;
			size_t reservedBytes;

			static inline void * NEW(size_t size) noexcept(false) {void * const res = malloc(size); if(res) return res; throw std::bad_alloc();}
			static inline void * CALLOC(size_t num, size_t size) noexcept(false) {void * const res = calloc(num, size); if(res) return res; throw std::bad_alloc();}
			static inline void DELETE(void * ptr) noexcept {free(ptr);}
			static inline void * REALLOC(void * ptr, size_t size) noexcept(false) {void * const res = realloc(ptr, size); if(res) return res; throw std::bad_alloc();}

			static inline bool GetUrlEncMapPos(unsigned char pos) {return pos >= 128 ? true : (UrlEncBitmap[pos/64] & (1 << pos));}

			/**
			 * @brief Size of the copies counter part of the reserved memory
			 */
			static const size_t COPIES_SIZE;
			static uint64_t const UrlEncBitmap[2];
			static char const zeroTermination;
		};

		/**
		 * Generates a hash value for the given string
		 *
		 * Helper function for hash tables. Use PeoplezString::HashValue() instead
		 *
		 * @param str String to get hash value for
		 *
		 * @return hash value of the given string
		 */
		inline size_t hash_value(PeoplezString const &str) noexcept {return str.HashValue();}
		/**
		 * Writes a sting to an output stream
		 *
		 * @param os Output stream to write in
		 * @param dt String to write to the stream
		 *
		 * @return Reference to os
		 */
		std::ostream& operator<<(std::ostream& os, PeoplezString const & dt) noexcept(false);
	} // namespace String
} // namespace Peoplez

namespace std
{
	template<>
	struct hash<Peoplez::String::PeoplezString>
	{
		size_t operator() (Peoplez::String::PeoplezString const & str) const
		{
			return str.HashValue();
		}
	};
} // namespace std

#endif // PEOPLEZ_STRING_PEOPLEZSTRING_H_
