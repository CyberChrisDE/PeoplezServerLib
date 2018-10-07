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

#ifndef PEOPLEZ_SYSTEM_IO_DATABASE_DATABASEMANAGER_H_
#define PEOPLEZ_SYSTEM_IO_DATABASE_DATABASEMANAGER_H_

// Local includes
#include "../../../String/PeoplezString.hpp"
#include "../../../General/Patterns/Factory.hpp"

//Extern includes
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <memory>

// External namespaces
using namespace sql;

namespace Peoplez
{
	namespace System
	{
		namespace IO
		{
			namespace Database
			{
				class DatabaseConnectionsHolder;

				/**
				 * @brief Module for all operation depending on the database
				 */
				class DatabaseManager final : public General::Patterns::Factory
				{
				public:
					/**
					 * Only constructor
					 *
					 * Creates the driver and the connection to the database and sets the schema
					 *
					 * @param server Url of the server on which the mysql server runs (for example: "localhost", "example.org")
					 * @param database The name of the database you want to connect to
					 * @param dbUserID The name of the user for authentication
					 * @param dbPassword The password of the user for authentication
					 */
					DatabaseManager(char const * server, char const * database, char const * dbUserID, char const * dbPassword) __attribute__((nonnull));
					/**
					 * Destructor
					 *
					 * Closes the connection
					 */
					virtual ~DatabaseManager() noexcept;
					/**
					 * Creates a separate connection for one more thread
					 */
					void InitThreadData() noexcept;
					/**
					 * Destroys one separate connection for a thread
					 */
					void CleanUpThreadData() noexcept;
					std::unique_ptr<General::Patterns::Product> CreateProduct();
					/**
					 * Creates a normal statement
					 *
					 * Creates a normal statement. Not to use for requests including parameters! "requestMutex" must be locked while existing!
					 *
					 * @return Shared pointer including the statement
					 */
					std::shared_ptr<sql::Statement> GetStatement()
					{
						return std::shared_ptr<sql::Statement>(conn->createStatement());
					}
					/**
					 * Creates a prepared statement
					 *
					 * Creates a prepared statement for use with parameters. "requestMutex" must be locked while existing!
					 *
					 * @return Shared pointer including the prepared statement
					 */
					std::shared_ptr<sql::PreparedStatement> GetPreparedStatement(SQLString command)
					{
						return std::shared_ptr<sql::PreparedStatement>(conn->prepareStatement(command));
					}

				private:
					Driver * driver = 0;
					Connection * conn = 0;
				};

				/**
				 * @brief Ensures one more open thread independent database connection
				 */
				class DatabaseConnectionHolder final : public General::Patterns::Product
				{
				public:
					/**
					 * Constructor
					 *
					 * @param database Database to hold a connection for
					 */
					DatabaseConnectionHolder(DatabaseManager &database) : db(database)
					{
						db.InitThreadData();
					}

					virtual ~DatabaseConnectionHolder()
					{
						db.CleanUpThreadData();
					}

				private:
					// Avoid copy
					DatabaseConnectionHolder(DatabaseConnectionHolder &) = delete;

					DatabaseManager &db;
				};
			} // namespace Database
		}
	}
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_IO_DATABASE_DATABASEMANAGER_H_
