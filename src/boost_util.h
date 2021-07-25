#ifndef __U_BOOST_UTIL__
#define __U_BOOST_UTIL__

//Add options 
// -DBOOST_LOG_DYN_LINK -std=c++11
// -lboost_log -lboost_log_setup -lboost_thread
// needs c++11 compiler

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/expressions/attr.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>

#define _le BOOST_LOG_TRIVIAL(error)
#define _li BOOST_LOG_TRIVIAL(info)
#define _ld BOOST_LOG_TRIVIAL(debug)
#define _lt BOOST_LOG_TRIVIAL(trace)

class boost_util {
public:
	static void set_log_level(const std::string& level, int to_file);
};

#endif
