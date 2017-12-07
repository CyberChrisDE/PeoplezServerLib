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

// Local includes
#include "DatabaseManager.hpp"
#include "../../Logging/Logger.hpp"

// Extern includes
#include <memory>

namespace Peoplez
{
	// Local namespaces
	using namespace General::Patterns;
	using namespace System::Logging;

	namespace Database
	{
		DatabaseManager::DatabaseManager(char const * const server, char const * const database, char const * const dbUserID, char const * const dbPassword)
		{
			try
			{
				driver = sql::mysql::get_driver_instance();

				conn = driver->connect(server, dbUserID, dbPassword);
				conn->setSchema(database);
				Logger::LogEvent("DatabaseManager initialized");
			}
			catch(sql::SQLException & e)
			{
				delete conn;

				throw;
			}
			catch(...)
			{
				delete conn;

				throw;
			}
		}

		DatabaseManager::~DatabaseManager() noexcept
		{
			try
			{
				delete conn;

				Logger::LogEvent("DatabaseManager Stopped");
			}
			catch(...)
			{
				Logger::LogException("Error while stopping DatabaseManager", __FILE__, __LINE__);
			}
		}

		void DatabaseManager::InitThreadData() noexcept
		{
			try
			{
				driver->threadInit();
			}
			catch (...)
			{
				Logger::LogException("Error in DatabaseManager::InitThreadData", __FILE__, __LINE__);
			}
		}

		void DatabaseManager::CleanUpThreadData() noexcept
		{
			try
			{
				driver->threadEnd();
			}
			catch(...)
			{
				Logger::LogException("Error in DatabaseManager::CleanUpThreadData", __FILE__, __LINE__);
			}
		}

		std::unique_ptr<Product> DatabaseManager::CreateProduct()
		{
			return std::unique_ptr<Product>(new DatabaseConnectionHolder(*this));
		}
	} // namespace Database
} // namespace Peoplez
