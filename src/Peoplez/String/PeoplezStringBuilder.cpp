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
#include "PeoplezStringBuilder.hpp"

// Local includes
#include "../System/Logging/Logger.hpp"
#include "BaseElement.hpp"
#include "ExtStringElement.hpp"

namespace Peoplez
{
	// Local namespaces
	using namespace System::Logging;

	namespace String
	{
		/**
		 * @brief The escape symbol for PeoplezStringBuilder
		 * @details The escape symbol that has to be at the beginning of every replacing inside the original text
		 */
		#define ESCAPE_SEQUENCE '#'

		PeoplezStringBuilder::PeoplezStringBuilder(PeoplezString text, PeoplezString const * const _replaceables, int const _repLen) : data(PeoplezStringBuilderData())
		{
			try
			{
				if(_repLen > 0)
				{
					data.SetReplaceables(_replaceables, _repLen);
					Create(text);
				}
				else
				{
					data.elements = std::shared_ptr<Element>(new BaseElement(text));
				}
			}
			catch(...)
			{
				Logger::LogException("Error in PeoplezStringBuilder Constructor", __FILE__, __LINE__);
			}
		}

		PeoplezStringBuilder::PeoplezStringBuilder(PeoplezStringBuilder const & original)
		{
			boost::unique_lock<boost::mutex> lockOriginal(original.dataLock);

			data = original.data;
		}

		PeoplezStringBuilderData const PeoplezStringBuilder::GetDataObject() const
		{
			// Lock data object
			boost::unique_lock<boost::mutex> lock(dataLock);

			return data;
		}

		void PeoplezStringBuilder::Create(PeoplezString const text)
		{
			try
			{
				size_t found = text.Find(ESCAPE_SEQUENCE);
				size_t foundLast = 0;
				int i = 0;
				std::shared_ptr<Element> elements;

				while(found != PeoplezString::NPOS && foundLast < text.Length())
				{
					for(i = 0; i < data.len; ++i) //Suchen nach dem passenden Replaceable
					{
						if(text.EqualTo(found + 1, data.replaceables[i].Length(), data.replaceables[i])) break;
					}

					if(i < data.len)
					{
						if(!elements.get()) elements = std::shared_ptr<Element>(new BaseElement(text.Substring(0, found))); //Erstes Element
						else if(found > foundLast) elements.get()->Append(new BaseElement(text.Substring(foundLast, found - foundLast))); //Element zwischen 2 Einträgen
						elements.get()->Append(new ExtStringElement(i));
						foundLast = found + data.replaceables[i].Length() + 1;
					}

					found = text.Find(ESCAPE_SEQUENCE, found + 1);
				}

				if(!elements.get()) elements = std::shared_ptr<Element>(new BaseElement(text));
				else if(foundLast < text.Length()) elements.get()->Append(new BaseElement(text.Substring(foundLast)));

				data.elements = elements;
			}
			catch(...)
			{
				Logger::LogException("Error in PeoplezStringBuilder::Create", __FILE__, __LINE__);
			}
		}

		PeoplezString PeoplezStringBuilder::ToString(PeoplezStringBuilderData const & data)
		{
			try
			{
				if(!data.elements.get()) return PeoplezString();

				const int len = data.elements.get()->Length(data.replacings) + 1;
				PeoplezString result;
				result.Resize(len);

				data.elements.get()->Create(data.replacings, &result);
				return result;
			}
			catch(...)
			{
				Logger::LogException("Error in PeoplezStringBuilder::ToString", __FILE__, __LINE__);
			}

			return PeoplezString();
		}

		void PeoplezStringBuilder::operator =(PeoplezStringBuilder rhs)
		{
			boost::unique_lock<boost::mutex> lockOwn(dataLock);
			boost::unique_lock<boost::mutex> lockRhs(rhs.dataLock);

			data = rhs.data;
		}

		StringBuilder::StringBuilder(PeoplezString text, PeoplezString * _replaceables, size_t _len) : len(_len), replaceables(new PeoplezString[_len]), replacings(0)
		{
			if(_len > 0)
			{
				for(size_t i = 0; i < _len; ++i) replaceables[i] = _replaceables->UniqueCopy();

				Create(text);
			}
			else elements = std::shared_ptr<Element>(new BaseElement(text));
		}

//		StringBuilder::StringBuilder(StringBuilder const & other)
//		{
//
//		}

		void StringBuilder::Create(PeoplezString text)
		{
			size_t const numEsc = text.Count(ESCAPE_SEQUENCE);

			if(__builtin_expect(numEsc > 0, true))
			{
				// TODOPeoplez Implement!!!
			}
		}

		PeoplezString StringBuilder::ToString()
		{
			try
			{
				if(__builtin_expect(elements != 0, true))
				{
					PeoplezString result(elements->Length(replacings));

					elements->Create(replacings, &result);

					return result;
				}
				else return PeoplezString();
			}
			catch(...)
			{
				Logger::LogException("Unknown error in StringBuilder::ToString", __FILE__, __LINE__);
			}

			return PeoplezString();
		}
	} // namespace String
} // namespace Peoplez

