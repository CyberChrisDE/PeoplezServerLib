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

#ifndef PEOPLEZ_SYSTEM_ARGUMENTS_ARGUMENT_HPP_
#define PEOPLEZ_SYSTEM_ARGUMENTS_ARGUMENT_HPP_

// External includes
#include <vector>
#include <string>

namespace Peoplez
{
	namespace System
	{
		namespace Arguments
		{
			/**
			 * @brief Instances represent possible program arguments
			 */
			class Argument
			{
			public:
				Argument() : m_isSet(0), m_names() {}
				/**
				 * Constructor
				 *
				 * @param names Names/Identifiers/Keys of the argument
				 */
				Argument(std::vector<std::string> const & names) : m_isSet(0), m_names(names) {}
				Argument(std::vector<std::string> && names) : m_isSet(0), m_names(std::move(names)) {}
				virtual ~Argument() {}

				/**
				 * Read method for argument list of main function
				 *
				 * @param argc Size of argv
				 * @param argv Copy of the argc parameter of the main function
				 */
				virtual int Read(int argc, char const * const argv[]) = 0;
				/**
				 * Getter for the names
				 */
				inline std::vector<std::string> const & GetNames() const {return m_names;}
				/**
				 * Checks whether this program argument is set
				 *
				 * @return true if is set; false otherwise
				 */
				inline bool IsSet() const noexcept {return m_isSet;}

			protected:
				/**
				 * @brief Indicates whether the argument is set
				 */
				bool m_isSet;

			private:
				std::vector<std::string> const m_names;
			};
		} // namespace Arguments
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_ARGUMENTS_ARGUMENT_HPP_
