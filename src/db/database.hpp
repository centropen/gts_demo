/*
 * database.hpp
 *
 *  Created on: Jul 5, 2015
 *      Author: root
 */

#ifndef DB_DATABASE_HPP_
#define DB_DATABASE_HPP_

#include <string>
#include <vector>
#include <sqlite3.h>
#include <iostream>

namespace gts {
namespace db {

class Database
{
public:
	Database(const char* filename);
	~Database();

	bool open(const char* filename);
	std::vector<std::vector<std::string> > query(const char* );
	void close();

	inline bool isError() {
		return (sqlite3_errmsg(_db) != std::string("not an error"));
	}

private:
	sqlite3 *_db;
};

} // db
} // gts

#endif /* DB_DATABASE_HPP_ */

