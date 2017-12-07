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

#ifndef PEOPLEZ_SYSTEM_COMMANDLINE_COMMANDMANAGER_HPP_
#define PEOPLEZ_SYSTEM_COMMANDLINE_COMMANDMANAGER_HPP_

// Local includes
#include "../../String/PeoplezString.hpp"

// Extern includes
#include <boost/thread.hpp>
#include <memory>

namespace Peoplez
{
	namespace System
	{
		namespace Commandline
		{
			/**
			 * @brief Manager class for console commands
			 */
			class CommandManager final
			{
			private:
				class CommandObject;
				/**
				 * @var typedef std::shared_ptr<CommandObject> CommandNode
				 * @brief Shared pointer to commandObject
				 */
				typedef std::shared_ptr<CommandObject> CommandNode;

				/**
				 * @brief Represents a node of the command tree
				 */
				class CommandObject final
				{
				public:
					CommandObject();
					/**
					 * Constructor taking command name and target function
					 *
					 * @param name Name of the command
					 * @param newTarget Target function of the command
					 */
					CommandObject(String::PeoplezString name, void (*newTarget)() = 0);
					/**
					 * Prints the names of all child commands to the standard output
					 */
					void PrintChilds();
					/**
					 * Adds the given command to the list of child commands
					 *
					 * @param val Command that should get added
					 */
					void Add(CommandNode val);
					/**
					 * Sets the new target for the command a the given command path
					 *
					 * @param location Path to the command
					 * @param newTarget Target to be set for the command
					 */
					void Add(std::vector<String::PeoplezString> location, void (*newTarget)());
					/**
					 * Adds the given command to the child commands of the command at the given command path
					 *
					 * @param location Path to the command to that the new command should get added
					 * @param val Command that sould get added
					 */
					void Add(std::vector<String::PeoplezString> location, CommandNode val);
					/**
					 * Calls the target at the given command path
					 *
					 * Calls the target function at the given command path if it is a command path given. If it is given a command path with a terminating '?' the childs of the command at the command path get printed. Otherwise an error is printed.
					 *
					 * @param commands Path to the command
					 */
					void ProcessCommand(std::vector<String::PeoplezString> commands);
					/**
					 * Getter for the child command with the given name
					 *
					 * @param key Name of the child command
					 *
					 * @return Shard pointer to 0 if not found, pointer to the child if found.
					 */
					CommandNode Get(String::PeoplezString key);
					/**
					 * @brief Descructor
					 */
					virtual ~CommandObject() {}
					/**
					 * @brief Name of the command
					 * @details Name of the command with that the command can be called
					 */
					String::PeoplezString Name;

				private:
					std::list<CommandNode> childs;
					void (*Target)();
				};

			public:
				/**
				 * Initializes the CommandManager
				 */
				static void Initialize();
				/**
				 * Reads console inputs and calls the precessoring function
				 */
				static void Read();
				/**
				 * Cleans up the CommandManager
				 */
				static void Close();
			private:
				static CommandObject tree;
				static boost::thread th;
				static int stopfd;
			};
		} // namespace Commandline
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_COMMANDLINE_COMMANDMANAGER_HPP_
