#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "profile_timer.h"
#include "common_v4l2.h"
#include "StdInThread.h"

using namespace std;

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

int main(void) {
	common_v4l2 v4;
	char *dev_name = "/dev/video0";
	struct buffer *buffers;
	unsigned int
		i,
		x_res = 640,
		y_res = 360
	;

	v4.init(dev_name, x_res, y_res);

	StdInThread stdin_thread;
	stdin_thread.start(NULL);

	int count0 = 0;
	int count1 = 0;

	while(true){
		profile_timer::count_fps();
		float fps = profile_timer::get_fps();
		if(++count0 % 100 == 0){
			cout << fps << endl;
			if(v4.fetched_.size() > 0)
			save_ppm(++count1, x_res, y_res, v4.fetched_[0]->size(), &(*(v4.fetched_[0].get()))[0] );
		}

		v4.update_image();

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
	/*
	for (i = 0; i < 30; i++) {
		v4.update_image();
		save_ppm(
			i,
			x_res,
			y_res,
			v4.get_image_size(),
			v4.get_image()
		);
	}
	*/
	v4.deinit();
	return 0;
}
