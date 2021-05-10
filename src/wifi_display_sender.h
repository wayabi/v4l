#ifndef __U_WIFI_DISPLAY_SENDER__
#define __U_WIFI_DISPLAY_SENDER__

#include <string>
#include <memory>
#include "tcp_sender.h"

class wifi_display_sender {
public:
	wifi_display_sender(int id, const std::string& ip_address, int port);
	void send(unsigned char id, int w, int h, std::shared_ptr<std::vector<char>> data);

private:
	unsigned char id_;
	std::shared_ptr<tcp_sender> tcp_sender_;

};
#endif
