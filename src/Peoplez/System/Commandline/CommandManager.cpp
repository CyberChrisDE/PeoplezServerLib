/**
 * Copyright 2017, 2019 Christian Geldermann
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
#include "../Commandline/CommandManager.hpp"

// Local includes
#include "../../System/Logging/Logger.hpp"
//#include "../../Services/Http/PageManager.hpp"
//#include "../../Services/Http/ServiceInterfaces/SearchServiceInterface.hpp"
//#include "../../Services/Http/ServiceInterfaces/UploadServiceInterface.hpp"

// Extern includes
#include <limits>
extern "C"
{
#include <sys/eventfd.h>
}

namespace Peoplez
{
	// Local namespaces
	//using namespace Services::Http;
	using namespace String;

	namespace System
	{
		// Local namespaces
		using namespace Logging;

		namespace Commandline
		{
			CommandManager::CommandObject CommandManager::tree;
			std::thread CommandManager::th;
			int CommandManager::stopfd = eventfd(0, EFD_NONBLOCK);

			void CommandManager::Initialize()
			{
				try
				{
					/*
					//TODO tree.Add(CommandNode(new CommandObject(PeoplezString("exit"), (void(*)())&Core::Close)));
					tree.Add(CommandNode(new CommandObject(PeoplezString("restart"))));

					//update
					CommandNode update(new CommandObject(PeoplezString("update")));
					tree.Add(update);
					{//update page
						CommandNode updatePage(new CommandObject(PeoplezString("page")));
						update->Add(updatePage);

						updatePage->Add(CommandNode(new CommandObject(PeoplezString("main"), PageManager::UpdateMainPage))); //MainPage
						updatePage->Add(CommandNode(new CommandObject(PeoplezString("search"), PageManager::UpdateSearchPage))); //SearchPage
						updatePage->Add(CommandNode(new CommandObject(PeoplezString("support"), PageManager::UpdateSupportPage))); //SupportPage
						updatePage->Add(CommandNode(new CommandObject(PeoplezString("choosing"), PageManager::UpdateChoosingPage))); //ChoosingPage
						updatePage->Add(CommandNode(new CommandObject(PeoplezString("login"), PageManager::UpdateLoginPage))); //LoginPage
						updatePage->Add(CommandNode(new CommandObject(PeoplezString("registration"), PageManager::UpdateRegistrationPage))); //RegistrationPage
						updatePage->Add(CommandNode(new CommandObject(PeoplezString("user"), PageManager::UpdateStandardPage))); //StandardPage
						updatePage->Add(CommandNode(new CommandObject(PeoplezString("document"), PageManager::UpdateDocumentPage))); //DocumentPage
						updatePage->Add(CommandNode(new CommandObject(PeoplezString("edit"), PageManager::UpdateEditPage))); //EditPage

						{//settings pages
							CommandNode updatePageSettings(new CommandObject(PeoplezString("settings"), PageManager::UpdatePageSpecificSettingsPage));
							updatePage->Add(updatePageSettings);

							updatePageSettings->Add(CommandNode(new CommandObject(PeoplezString("page"), PageManager::UpdatePageSettingsPage))); //PageSettingsPage
							updatePageSettings->Add(CommandNode(new CommandObject(PeoplezString("profile"), PageManager::UpdateProfileSettingsPage))); //ProfileSettingsPage
							updatePageSettings->Add(CommandNode(new CommandObject(PeoplezString("search"), PageManager::UpdateSearchSettingsPage))); //SearchSettingsPage
							updatePageSettings->Add(CommandNode(new CommandObject(PeoplezString("specific"), PageManager::UpdatePageSpecificSettingsPage))); //PageSpecificSettingsPage
						}

						CommandNode services(new CommandObject(PeoplezString("service")));
						update->Add(services);

						services->Add(CommandNode(new CommandObject(PeoplezString("search"), ServiceInterfaces::SearchServiceInterface::SUpdate)));
						services->Add(CommandNode(new CommandObject(PeoplezString("upload"), ServiceInterfaces::UploadServiceInterface::SUpdate)));
					}
					{//others
						update->Add(CommandNode(new CommandObject(PeoplezString("menu"), PageManager::UpdateMenu))); //PageManager::Menu
					}
					*/

					th = std::thread(Read);

					Logger::LogEvent("CommandManager initialized");
				}
				catch(...)
				{
					Logger::LogException("Error in CommandManager::Initialize", __FILE__, __LINE__);
				}
			}

			void CommandManager::Read()
			{
				try
				{
					char commandCString[1000];
					fd_set constSet, set;
					FD_ZERO(&constSet);
					FD_SET(stopfd, &constSet);
					FD_SET(0, &constSet);
					timeval constTv = {0, 0}, tv;
					constTv.tv_sec = std::numeric_limits<decltype(constTv.tv_sec)>::max();

					while(true)
					{
						tv = constTv;
						set = constSet;
						select(stopfd + 1, &set, 0, 0, &tv);

						if(FD_ISSET(0, &set))
						{
							std::cin.getline(commandCString, 1000);
							PeoplezString commandString = PeoplezString(commandCString);
							std::vector<PeoplezString> commands;
							commandString.Split<true>(commands, ' ');
							tree.ProcessCommand(commands);
						}

						if(FD_ISSET(stopfd, &set)) break;
					}
				}
				catch(...)
				{
					Logger::LogException("Error in CommandManager::Read", __FILE__, __LINE__);
				}

				Logger::LogEvent("CommandManager closed");
			}

			void CommandManager::Close()
			{
				try
				{
					eventfd_write(stopfd, 1);
					th.join();
				}
				catch(...)
				{
					Logger::LogException("Error in CommandManager::Close", __FILE__, __LINE__);
				}
			}

			CommandManager::CommandObject::CommandObject() : Target(0)
			{
			}

			CommandManager::CommandObject::CommandObject(PeoplezString name, void (*newTarget)()) : Name(name), Target(newTarget)
			{
			}

			void CommandManager::CommandObject::PrintChilds()
			{
				for(std::list<std::shared_ptr<CommandObject> >::const_iterator childs_iter = childs.begin(); childs_iter != childs.end(); ++childs_iter)
				{
					std::cout << (*childs_iter)->Name << std::endl;
				}
			}

			void CommandManager::CommandObject::Add(CommandNode val)
			{
				childs.push_back(val);
			}

			void CommandManager::CommandObject::Add(std::vector<String::PeoplezString> location, void (*newTarget)())
			{
				if(location.size() > 0)
				{
					location.erase(location.begin());
					Get(location[0])->Add(location, newTarget);
				}
				else
				{
					Target = newTarget;
				}
			}

			void CommandManager::CommandObject::Add(std::vector<String::PeoplezString> location, CommandNode val)
			{
				if(location.size() > 0)
				{
					location.erase(location.begin());
					Get(location[0])->Add(location, val);
				}
				else
				{
					childs.push_back(val);
				}
			}

			void CommandManager::CommandObject::ProcessCommand(std::vector<PeoplezString> commands)
			{
				try
				{
					if(commands.size() > 0)
					{
						if(commands[0].EqualTo("?", 1)) PrintChilds();
						else
						{
							std::shared_ptr<CommandObject> obj = Get(commands[0]);

							if(!obj) std::cout << "Command not found" << std::endl;
							else
							{
								commands.erase(commands.begin());
								obj->ProcessCommand(commands);
							}
						}
					}
					else if(!Target)
					{
						std::cout << "Command not found" << std::endl;
					}
					else
					{
						Target();
						std::cout << "Command processed" << std::endl;
					}
				}
				catch(...)
				{
					Logger::LogException("Error in CommandObject::ProcessCommand", __FILE__, __LINE__);
				}
			}

			CommandManager::CommandNode CommandManager::CommandObject::Get(PeoplezString key)
			{
				bool found = false;
				std::shared_ptr<CommandObject> obj;

				for(std::list<std::shared_ptr<CommandObject> >::const_iterator childs_iter = childs.begin(); childs_iter != childs.end(); ++childs_iter)
				{
					obj = (*childs_iter);
					if(obj->Name == key)
					{
						found = true;
						break;
					}
				}

				if(found) return obj;
				else return std::shared_ptr<CommandObject>((CommandObject*)0);
			}
		} // namespace Commandline
	} // namespace System
} // namespace Peoplez
