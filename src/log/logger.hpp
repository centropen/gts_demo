/*
 * logger.hpp
 *
 *  Created on: Jul 5, 2015
 *      Author: root
 */

#ifndef LOG_LOGGER_HPP_
#define LOG_LOGGER_HPP_

#define BOOST_LOG_DYN_LINK 1 // or as linking attribute

#include <stdexcept>
#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/filesystem.hpp>

#include <boost/log/core.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

namespace gts {
namespace log {



namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace exp = boost::log::expressions;
namespace keywords = boost::log::keywords;

//should be moved to ini config file
const char* tmp_log = "/tmp/log/";


// severity
enum severity_level {
	info,
	debug,
	trace,
	warning,
	error,
	fatal
};

struct DirectoryOperation {
	explicit DirectoryOperation()
	{}
	static bool exists(const char* path) {
		return boost::filesystem::exists(path);
	}

	static void create(const char* path) {
		boost::filesystem::create_directories(path);
	}
};

typedef sinks::asynchronous_sink < sinks::text_file_backend > file_sink;

/**
 * sink to log file /tmp/log
 * @param sink
 */
void init_file_collecting(boost::shared_ptr< file_sink > sink)
{
	// check if /tmp/log exists, if not, create it
	if (!DirectoryOperation::exists(tmp_log))
		DirectoryOperation::create(tmp_log);

	sink->locked_backend()->set_file_collector(sinks::file::make_collector(
			keywords::target = tmp_log,
			keywords::max_size = 8 * 1024 * 1024, // max size of files in bytes
			keywords::min_free_space = 100 * 1024 * 1024 // min free space on disk in bytes
	));
}

void write_header(sinks::text_file_backend::stream_type& file)
{
	file << "<*********General*Trading*System**Demo*********>\n";
	file << "*********************START**********************\n";
}

void write_footer(sinks::text_file_backend::stream_type& file)
{
	file << "**********************END***********************\n";
	file << "<*********General*Trading*System**Demo*********>\n";
}
void init_logging()
{
	boost::shared_ptr< file_sink > sink(new file_sink(
			keywords::file_name = "GTS_%Y%m%d_%H%M%S_%5N.log",
			keywords::rotation_size = 8192 // rotation in characters
	));
	sink->set_formatter(
			exp::format("record id=\"%1%\" timestamp=\"%2%\": %3%")
				% exp::attr< unsigned int >("RecordID")
				% exp::attr< boost::posix_time::ptime >("TimeStamp")
				% exp::smessage
				% exp::xml_decor[ exp::stream << exp::smessage ]
	); //decorated message
	// header and footer writing functors
	sink->locked_backend()->set_open_handler(&write_header);
	sink->locked_backend()->set_close_handler(&write_footer);
}


} //log
} //gts
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger, gts::log::src::logger)

#define START_LOGGING  											\
	gts::log::init_logging();									\
	gts::log::logging::core::get()->add_global_attribute(		\
		"TimeStamp", gts::log::attrs::local_clock());			\
		gts::log::logging::core::get()->add_global_attribute(	\
		"RecordID", gts::log::attrs::counter<unsigned int>());	//\
	//gts::log::src::logger lg;
#define LOG_INFO BOOST_LOG(logger::get()) // note

#endif /* LOG_LOGGER_HPP_ */
