#ifndef COMMON_V4L2_H
#define COMMON_V4L2_H

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>

#include <libv4l2.h>
#include <linux/videodev2.h>

#define COMMON_V4L2_CLEAR(x) memset(&(x), 0, sizeof(x))

typedef struct {
	void *start;
	size_t length;
} CommonV4l2_Buffer;

class common_v4l2 {
public:
	static void xioctl(int fh, unsigned long int request, void *arg);
	void init(char *dev_name, unsigned int x_res, unsigned int y_res);
	void update_image();
	char* get_image();
	size_t get_image_size();
	void deinit();

private:
	int fd_;
	CommonV4l2_Buffer* buffers_;
	struct v4l2_buffer buf_;
	unsigned int n_buffers_;
};



#endif
