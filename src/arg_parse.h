#ifndef __U_ARG_PARSE__
#define __U_ARG_PARSE__

#include <string>
#include <ostream>

#include <boost/program_options.hpp>

class arg_parse {

public:
	std::string ip_address;
	int port;
	int exposure;
	int brightness;
	int contrast;
	int colour;
public:
	std::string parse(int argc, char** argv)
	{
		using namespace boost::program_options;
		options_description opt("opt");
		opt.add_options()
			("ip_address", value<std::string>()->required(), "ip_address")
			("port", value<int>()->default_value(12345), "port")
			("exposure", value<int>()->default_value(-1), "exposure")
			("brightness", value<int>()->default_value(0), "brightness")
			("contrast", value<int>()->default_value(50), "contrast")
			("colour", value<int>()->default_value(0), "colour")
		;
		variables_map vm;
		try{
			store(parse_command_line(argc, argv, opt), vm);
			notify(vm);
			ip_address = vm["ip_address"].as<std::string>();
			port = vm["port"].as<int>();
			exposure = vm["exposure"].as<int>();
			brightness = vm["brightness"].as<int>();
			contrast = vm["contrast"].as<int>();
			colour = vm["colour"].as<int>();
		}catch(std::exception& e){
			return std::string(e.what());
		}
		return std::string();
	}

	void print(std::ostream& o)
	{
		o << "ip_address[std::string]: " << ip_address << std::endl;
		o << "port[int]: " << port << std::endl;
		o << "exposure[int]: " << exposure << std::endl;
		o << "brightness[int]: " << brightness << std::endl;
		o << "contrast[int]: " << contrast << std::endl;
		o << "colour[int]: " << colour << std::endl;
	}
};
#endif
