/*
 * mocks.hpp
 *
 *  Created on: Jul 5, 2015
 *      Author: root
 */

#ifndef TEST_MOCKS_HPP_
#define TEST_MOCKS_HPP_

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Suites
#include <boost/test/unit_test.hpp>
// test cases
#include "../db/database.hpp"

namespace gts {
namespace test {

BOOST_AUTO_TEST_SUITE(db)
BOOST_AUTO_TEST_CASE(database_test)
{
	gts::db::Database *db;
	db = new gts::db::Database("Database.sqlite");
	const char *sql = "CREATE TABLE STOCKQUOTE ("	\
				"ID UNSIGNED BIGINT KEY NOT NULL,"	\
				"UUID TEXT KEY NOT NULL,"			\
				"SYMBOL CHAR(50),"					\
				"TIMESTAMP REAL,"					\
				"OPEN DOUBLE,"						\
				"HIGH DOUBLE,"						\
				"LOW DOUBLE,"						\
				"CLOSE DOUBLE,"						\
				"VOLUME BIGINT,"					\
				"ADJUSTEDCLOSE DOUBLE);";
	db->query(sql);
	BOOST_REQUIRE( db->isError() == false);

	db->query("INSERT INTO STOCKQUOTE VALUES "		\
			"(1, 'd866a824-a880-4953-8fed-6ad27d5cb69d', 'GOOG', 20150321032100,"	\
			"554.89, 554.80, 558.42, 550.10,"		\
			"2435, 177.57);");

	BOOST_CHECK( db->isError() == false ); //continues

	typedef std::vector<std::vector <std::string> > Tresult;
	Tresult result;
	result = db->query("SELECT 1, 2, 3 FROM STOCKQUOTE;");
	typedef Tresult::const_iterator iter;
	iter begin = result.begin();
	for(; iter!= result.end(); ++iter)
	{
		std::vector<std::string> row = *iter;
		BOOST_CHECK_MESSAGE(row.at(0).length()!=0, "ID= " << row.at(0));
		BOOST_CHECK_MESSAGE(row.at(1).length()!=0, "UUID= " << row.at(1));
		BOOST_CHECK_MESSAGE(row.at(2).length()!=0, "Symbol= " << row.at(2));
	}
	db->close();

	BOOST_CHECK_EQUAL( db->isError(), false);
}
BOOST_AUTO_TEST_SUITE_END()

} // test
} // gts



#endif /* TEST_MOCKS_HPP_ */
