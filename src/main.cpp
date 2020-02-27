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

void decode(std::shared_ptr<vector<char>> d, int w, int h)
{
	Mat m(h, w, CV_8UC3);
	jpeg_decoder::decode((unsigned char*)(m.data), w*h*3, (unsigned char*)(&(*(d.get()))[0]), d->size());
}

int main(void) {
	common_v4l2 v4;
	char *dev_name = "/dev/video0";
	struct buffer *buffers;
	unsigned int
		i,
		x_res = 640,
		y_res = 480 
	;

	v4.init(dev_name, x_res, y_res);

	StdInThread stdin_thread;
	stdin_thread.start(NULL);

	asio::io_service io_service_;
	thread_pool pool_(io_service_, 3);
	
	int count0 = 0;
	int count1 = 0;

	while(true){
		profile_timer::count_fps();
		float fps = profile_timer::get_fps();
		if(++count0 % 100 == 0){
			cout << fps << endl;
			//if(v4.fetched_.size() > 0)
			//save_ppm(++count1, x_res, y_res, v4.fetched_[0]->size(), &(*(v4.fetched_[0].get()))[0] );
			
		}

		v4.update_image();

		if(v4.fetched_.size() > 0){
			pool_.post(std::bind(decode, v4.fetched_[0], x_res, y_res));
			v4.fetched_.pop_front();
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
