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

#ifndef PEOPLEZ_SYSTEM_LOGGING_LOGGER_H_
#define PEOPLEZ_SYSTEM_LOGGING_LOGGER_H_

// Local includes
#include "../../String/PeoplezString.hpp"

// Extern includes
#include <boost/thread/mutex.hpp>
#include <iostream>

namespace Peoplez
{
	namespace System
	{
		namespace Logging
		{
			/**
			 * @brief Logs information and exception messages
			 */
			class Logger
			{
			public:
				/**
				 * Initializes the Logger
				 */
				static void Initialize() noexcept {}

				/**
				 * Cleans up the Logger
				 */
				static void Stop() noexcept {}

				/**
				 * Logs an information
				 *
				 * @param text Text to be logged
				 *
				 * @par Exception Safety
				 *  No-throw guarantee
				 */
				static void LogEvent(char const * const text) noexcept
				{
					try
					{
						boost::unique_lock<boost::mutex> const lock(console_lock);
						std::cout << "Log:\t" << text << std::endl;
					}
					catch(...) {}
				}

				/**
				 * Logs an information

				 * @param text Text to be logged
				 *
				 * @par Exception safety
				 *  No-throw guarantee
				 */
				static void LogEvent(String::PeoplezString const text) noexcept
				{
					try
					{
						boost::unique_lock<boost::mutex> const lock(console_lock);
						std::cout << "Log:\t" << text << std::endl;
					}
					catch(...) {}
				}

				/**
				 * Logs an error message
				 *
				 * @param text Error text to be logged
				 * @param file Name of the file the error was thrown
				 * @param line Line number in which the error was thrown
				 *
				 * @par Exception safety
				 *  No-throw guarantee
				 */
				static void LogException(char const * const text, char const * const file, uint32_t const line) noexcept
				{
					try
					{
						boost::unique_lock<boost::mutex> const lock(console_lock);
						if(line > 0) std::cout << "Exception:\t" << text << "(" << file << ":" << line << ")" << std::endl;
						else std::cout << "Exception:\t" << text << std::endl;
					}
					catch(...) {}
				}

				/**
				 * Logs an error message
				 *
				 * @param text Error text to be logged
				 * @param file Name of the file the error was thrown
				 * @param line Line number in which the error was thrown
				 *
				 * @par Exception safety
				 * 	No-throw guarantee
				 */
				static void LogException(String::PeoplezString const text, char const * const file, uint32_t const line) noexcept
				{
					try
					{
						boost::unique_lock<boost::mutex> lock(console_lock);
						if(line > 0) std::cout << "Exception:\t" << text << "(" << file << ":" << line << ")" << std::endl;
						else std::cout << "Exception:\t" << text << std::endl;
					}
					catch(...) {}
				}

				//static void LogDBChange(...);

				/**
				 * @brief Mutex for prevent writing with several threads at the same time
				 */
				static boost::mutex console_lock;
			};
		} // namespace Logging
	} // namespace System
} // namesapce Peoplez

#endif // PEOPLEZ_SYSTEM_LOGGING_LOGGER_H_
