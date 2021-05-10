#include "wifi_display_sender.h"
#include <memory>
#include <vector>
#include <iostream>
#include <memory.h>
#include <opencv2/imgcodecs.hpp>

using namespace std;

wifi_display_sender::wifi_display_sender(int id, const std::string& ip_address, int port)
{
	id_ = (unsigned char)id;
	tcp_sender_ = make_shared<tcp_sender>(ip_address, port);
	tcp_sender_->start();
}

void wifi_display_sender::send(unsigned char id, int w, int h, std::shared_ptr<std::vector<char>> data)
{
	if(tcp_sender_->get_num_queued() > 0) return;
	std::shared_ptr<std::vector<char>> buf = std::make_shared<std::vector<char>>();
	int size_header = 13;
	buf->resize(data->size()+size_header);
	char* p_data = &(*(data.get()))[0];
	char* p_buf = &(*(buf.get()))[0];
	memcpy(p_buf+0, &id, 1);
	memcpy(p_buf+1, &w, 4);
	memcpy(p_buf+5, &h, 4);
	int data_size = data->size();
	memcpy(p_buf+9, &data_size, 4);
	memcpy(p_buf+13, p_data, data_size);
	tcp_sender_->send_data(buf);
}

