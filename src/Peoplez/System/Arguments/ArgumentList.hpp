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

#ifndef PEOPLEZ_SYSTEM_ARGUMENTS_ARGUMENTLIST_HPP_
#define PEOPLEZ_SYSTEM_ARGUMENTS_ARGUMENTLIST_HPP_

// Local includes
#include "Argument.hpp"

// External includes
#include <list>
#include <string>
#include <map>

namespace Peoplez
{
	namespace System
	{
		namespace Arguments
		{
			/**
			 * @brief Container for a list of argument objects
			 */
			class ArgumentList final
			{
			public:
				ArgumentList() {}
				//virtual ~ArgumentList();

				/**
				 * Adds an argument to the list
				 *
				 * @param arg Argument to add to the list
				 */
				inline void AddArgument(Argument & arg) {AddArgument(&arg);}
				/**
				 * Adds an argument to the list
				 *
				 * @param arg Argument to add to the list
				 */
				void AddArgument(Argument * arg);
				/**
				 * Adds a list of arguments to the list
				 *
				 * @param args Arguments to add
				 */
				inline void AddArguments(std::vector<Argument *> & args) {for(std::vector<Argument *>::iterator i = args.begin(); i != args.end(); ++i) AddArgument(*i);}
				/**
				 * Adds a list of arguments to the list
				 *
				 * @param args Arguments to add
				 */
				inline void AddArguments(std::vector<Argument> & args) {for(std::vector<Argument>::iterator i = args.begin(); i != args.end(); ++i) AddArgument(*i);}
				/**
				 * Extracts arguments from the programs argument list
				 *
				 * @param argc Copy of argc parameter of main function
				 * @param argv Copy of argv parameter of main function
				 */
				void ReadArguments(int argc, char const * const argv[]);
				/**
				 * Getter for argument with a name
				 *
				 * The call "program 15 -port 80" or "program -port 80 15" has an argument with name "port" and value 80
				 * and an unnamed argument with value 15 (the second example could also be one argument with name "port"
				 * and values 80 and 15 and no unnamed one.
				 *
				 * @param name Name of the argument
				 */
				Argument * GetNamedArgument(std::string name);
				/**
				 * Getter for arguments withoud names
				 *
				 * The call "program 15 -port 80" or "program -port 80 15" has an argument with name "port" and value 80
				 * and an unnamed argument with value 15 (the second example could also be one argument with name "port"
				 * and values 80 and 15 and no unnamed one.
				 *
				 * @return List of all unnamed arguments
				 */
				inline std::list<std::string> GetUnnamedArguments() const {return m_unnamed_args;}

			private:
				//Forbid copies
				ArgumentList(ArgumentList const &) = delete;
				ArgumentList & operator=(ArgumentList const &) = delete;
				ArgumentList & operator=(ArgumentList &&) = delete;

				std::string m_program_name;
				std::list<Argument *> m_named_args;
				std::map<std::string, Argument *> m_name_map;
				std::list<std::string> m_unnamed_args;
			};
		} // namespace Arguments
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_ARGUMENTS_ARGUMENTLIST_HPP_
