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

#ifndef PEOPLEZSTRINGBUILDER_H_
#define PEOPLEZSTRINGBUILDER_H_

// Local includes
#include "PeoplezStringBuilderData.hpp"

// Extern includes
#include <boost/thread/shared_mutex.hpp>

namespace Peoplez
{
	namespace String
	{
		/**
		 * @brief Creator for texts that have text parts which should more often get replaced
		 */
		class PeoplezStringBuilder final
		{
		public:
			/**
			 * Standard constructor
			 */
			PeoplezStringBuilder() : data() {}
			/**
			 * Preferred constructor
			 *
			 * @param text Text to be modified
			 * @param replaceables List of text parts that should get replaced
			 * @param repLen Number of replaceables
			 */
			PeoplezStringBuilder(PeoplezString text, PeoplezString const * replaceables, int repLen);
			/**
			 * Copy constructor
			 *
			 * @param original The instance that should get copied
			 */
			PeoplezStringBuilder(PeoplezStringBuilder const & original);
			/**
			 * Getter for a new instance of the needed data object
			 *
			 * @return New instance of the object that takes the new text parts
			 */
			PeoplezStringBuilderData const GetDataObject() const;
			/**
			 * Copy assignment operator
			 *
			 * @param rhs The instance that should get copied
			 */
			void operator=(PeoplezStringBuilder rhs);
			/**
			 * Creates the target text
			 *
			 * Replaces the replaceables in the source text by their replacings and and returns the result
			 *
			 * @param data Data object containing the new text parts
			 * @return Text with replaced source text parts
			 */
			static PeoplezString ToString(PeoplezStringBuilderData const & data);
		private:
			/**
			 * Help function for the constructors
			 *
			 * @param text Text to be modified
			 */
			void Create(PeoplezString text);

			PeoplezStringBuilderData data;
			mutable boost::mutex dataLock;
		};

		struct NON_ENSURE_UNIQUE {};

		class StringBuilder final
		{
		public:
			StringBuilder() : len(0), replacings(0) {}
			StringBuilder(PeoplezString text, PeoplezString * replaceables, size_t len);
			StringBuilder(PeoplezStringBuilder const & other);
			StringBuilder & operator=(StringBuilder const & rhs);
			void SetReplacing(PeoplezString const & key, PeoplezString const & value);
			//void SetReplacing(PeoplezString const && key, PeoplezString const & value);
			//void SetReplacing(PeoplezString const & key, PeoplezString const && value);
			//void SetReplacing(PeoplezString const && key, PeoplezString const && value);
			void SetReplacing(size_t index, PeoplezString const & value);
			//void SetReplacing(size_t index, PeoplezString const && value);
			PeoplezString ToString();

		private:
			void Create(PeoplezString text);

			size_t len;
			std::shared_ptr<Element> elements;
			boost::shared_array<PeoplezString> replaceables;
			PeoplezString * replacings;
			//PeoplezString * predefs;
		};
	} // namespace String
} // namespace Peoplez

#endif // PEOPLEZSTRINGBUILDER_H_
