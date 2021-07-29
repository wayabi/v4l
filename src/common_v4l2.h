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

#include <vector>
#include <deque>
#include <memory>

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
	void init(char *dev_name, unsigned int x_res, unsigned int y_res, int fps);
	void update_image();
	void deinit();
	void set_exposure(int v);
	void set_brightness(int v);
	void set_hue(int v);
	void set_colour(int v);
	void set_contrast(int v);

public:
	std::deque<std::shared_ptr<std::vector<char>>> fetched_;

private:
	int fd_;
	CommonV4l2_Buffer* buffers_;
	struct v4l2_buffer buf_;
	unsigned int n_buffers_;

	const int size_buf_fetched_ = 100;
};



#endif
