/*
 * database.cpp
 *
 *  Created on: Jul 5, 2015
 *      Author: root
 */
#include "database.hpp"

namespace gts {
namespace db {

Database::Database(const char* filename)
{
	_db = nullptr;
	open(filename);
}

Database::~Database()
{
}

bool Database::open(const char* filename)
{
	return (sqlite3_open(filename, &_db) == SQLITE_OK);
}

std::vector<std::vector<std::string> > Database::query(const char* query)
{
	sqlite3_stmt *statement;
	std::vector<std::vector<std::string> > results;
	if (sqlite3_prepare_v2(_db, query, -1, &statement, 0) == SQLITE_OK)
	{
		int cols = sqlite3_column_count(statement);
		int result = 0;
		while (true) {
			result = sqlite3_step(statement);
			if (result == SQLITE_ROW) {
				std::vector<std::string> values;
				for (int col = 0; col < cols; col++)
				{
					std::string value;
					char *val = (char*)sqlite3_column_text(statement, col);
					if (val) value = val;
					else value = "";
					values.push_back(value);
				}
				results.push_back(values);
			} else break;
		}
		sqlite3_finalize(statement);
	}
	std::string error = sqlite3_errmsg(_db);
	if(error != "not an error")
		std::cout << query << " " << error << std::endl;

	return results;
}

void Database::close()
{
	sqlite3_close(_db);
}

} // db
} // gtss
