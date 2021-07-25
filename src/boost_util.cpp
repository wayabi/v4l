#include "boost_util.h"

void boost_util::set_log_level(const std::string& level, int to_file)
{
	namespace attrs = boost::log::attributes;
	namespace keywords = boost::log::keywords;

	boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");
	std::string format = "%TimeStamp% [%Severity%]%Message%";
	boost::log::add_console_log(std::cout, boost::log::keywords::format = format);

		//default log level
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
	if(level == "fatal"){
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::fatal);
	}else if(level == "error"){
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::error);
	}else if(level == "warning"){
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
	}else if(level == "info"){
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
	}else if(level == "debug"){
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
	}else if(level == "trace"){
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
	}

		namespace expr = boost::log::expressions;
	if(to_file != 0){
		boost::log::add_file_log(
			keywords::auto_flush = true,
			keywords::open_mode = std::ios::out | std::ios::app,
			keywords::target = "log",
			keywords::file_name = "log/%Y%m%d_%3N.log",
			keywords::rotation_size = 100*1024*1024,
			keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
			keywords::format = format
		);
	}
	boost::log::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
}

