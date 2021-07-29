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
	int w_camera;
	int h_camera;
	int fps;
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
			("w_camera", value<int>()->default_value(640), "w_camera")
			("h_camera", value<int>()->default_value(360), "h_camera")
			("fps", value<int>()->default_value(260), "fps")
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
			w_camera = vm["w_camera"].as<int>();
			h_camera = vm["h_camera"].as<int>();
			fps = vm["fps"].as<int>();
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
		o << "w_camera[int]: " << w_camera << std::endl;
		o << "h_camera[int]: " << h_camera << std::endl;
		o << "fps[int]: " << fps << std::endl;
	}
};
#endif
