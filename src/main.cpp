#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "profile_timer.h"
#include "common_v4l2.h"
#include "StdInThread.h"
#include "thread_pool.h"
#include "jpeg_decoder.h"
#include "wifi_display_sender.h"
#include "arg_parse.h"
#include "boost_util.h"

using namespace std;
using namespace boost;
using namespace cv;

static void save_ppm(
	unsigned int i,
	unsigned int x_res,
	unsigned int y_res,
	size_t data_lenght,
	char *data
) {
	FILE *fout;
	char out_name[256];

	sprintf(out_name, "./out/%05d.jpg", i);
	fout = fopen(out_name, "w");
	if (!fout) {
		perror("error: fopen");
		exit(1);
	}
	//fprintf(fout, "P6\n%d %d 255\n", x_res, y_res);
	fwrite(data, data_lenght, 1, fout);
	fclose(fout);
}

void decode(std::shared_ptr<vector<char>> d, int w, int h, int idx)
{
	Mat m(h, w, CV_8UC3);
	jpeg_decoder::decode((unsigned char*)(m.data), w*h*3, (unsigned char*)(&(*(d.get()))[0]), d->size());

	stringstream ss;
	ss << "./out/" << idx << ".bmp";
	imwrite(ss.str().c_str(), m);
}

void send_data(
	std::shared_ptr<wifi_display_sender> sender,
	unsigned char i,
	unsigned int x_res,
	unsigned int y_res,
	std::shared_ptr<std::vector<char>> data
) {
	sender->send(i, x_res, y_res, data);
}

int main(int argc, char** argv) {
	arg_parse args;
	string parse_err = args.parse(argc, argv);
	if(parse_err.size() > 0){
		_le << parse_err;
		return 1;
	}
	std::shared_ptr<wifi_display_sender> sender_ = std::make_shared<wifi_display_sender>(0, args.ip_address.c_str(), args.port);
	common_v4l2 v4;
	char *dev_name = "/dev/video0";
	struct buffer *buffers;
	unsigned int
		i,
		x_res = args.w_camera,
		y_res = args.h_camera 
	;

	v4.init(dev_name, x_res, y_res, args.fps);
	//v4.set_exposure(-1);
	//v4.set_brightness(100);
	//v4.set_contrast(58);
	//v4.set_colour(100);
	v4.set_exposure(args.exposure);
	v4.set_brightness(args.brightness);
	v4.set_contrast(args.contrast);
	v4.set_colour(args.colour);

	StdInThread stdin_thread;
	stdin_thread.start(NULL);

	asio::io_service io_service_;
	thread_pool pool_(io_service_, 1);
	
	int count0 = 0;
	int count1 = 0;

	while(true){
		profile_timer::count_fps();
		float fps = profile_timer::get_fps();

		v4.update_image();

		if(++count0 % 3 == 0){
			if(count0 % 100 == 0){
				cout << fps << endl;
			}
			if(v4.fetched_.size() > 0){
				//pool_.post(std::bind(decode, v4.fetched_[0], x_res, y_res, ++count1));
				//pool_.post(std::bind(save_ppm, ++count1, x_res, y_res, v4.fetched_[0]->size(), &(*(v4.fetched_[0].get()))[0]));
				pool_.post(std::bind(send_data, sender_, 0, x_res, y_res, v4.fetched_[0]));
				v4.fetched_.pop_front();
			}
		}

		stdin_thread.ss.lock();
		vector<char> c = stdin_thread.ss.head();
		stdin_thread.ss.erase();
		stdin_thread.ss.unlock();

		string s = Util::trim(string(&c[0], c.size()), "\n");
		vector<string> command = Util::split(s, ',');
		if(command.size() > 0){
			if(command[0] == "q"){
				stdin_thread.stop();
				break;
			}
		}
	}
	v4.deinit();
	return 0;
}
