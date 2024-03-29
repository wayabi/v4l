#include "common_v4l2.h"
#include <iostream>

using namespace std;

void common_v4l2::xioctl(int fh, unsigned long int request, void *arg)
{
	int r;
	do {
		r = v4l2_ioctl(fh, request, arg);
	} while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));
	if (r == -1) {
		fprintf(stderr, "error %d, %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void common_v4l2::init(char *dev_name, unsigned int x_res, unsigned int y_res, int fps) {
	enum v4l2_buf_type type;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_streamparm parm;
	parm.parm.capture.timeperframe.numerator = 1;
	parm.parm.capture.timeperframe.denominator = fps;
	unsigned int i;

	fd_ = v4l2_open(dev_name, O_RDWR | O_NONBLOCK, 0);
	if (fd_ < 0) {
		perror("Cannot open device");
		exit(EXIT_FAILURE);
	}
	COMMON_V4L2_CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width	   = x_res;
	fmt.fmt.pix.height	  = y_res;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SGRBG8;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_Y16;
	//fmt.fmt.pix.field	   = V4L2_FIELD_INTERLACED;
	fmt.fmt.pix.field	   = V4L2_FIELD_NONE;
	fmt.fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
	xioctl(fd_, VIDIOC_S_FMT, &fmt);
	if ((fmt.fmt.pix.width != x_res) || (fmt.fmt.pix.height != y_res))
		printf("Warning: driver is sending image at %dx%d\n",
			fmt.fmt.pix.width, fmt.fmt.pix.height);
	COMMON_V4L2_CLEAR(req);
	req.count = 2;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	xioctl(fd_, VIDIOC_REQBUFS, &req);
	buffers_ = (CommonV4l2_Buffer*)calloc(req.count, sizeof(*buffers_));

	for (n_buffers_ = 0; n_buffers_ < req.count; ++n_buffers_) {
		COMMON_V4L2_CLEAR(buf_);
		buf_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf_.memory = V4L2_MEMORY_MMAP;
		buf_.index = n_buffers_;
		xioctl(fd_, VIDIOC_QUERYBUF, &buf_);
		buffers_[n_buffers_].length = buf_.length;
		buffers_[n_buffers_].start = v4l2_mmap(NULL, buf_.length,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd_, buf_.m.offset);
		if (MAP_FAILED == buffers_[n_buffers_].start) {
			perror("mmap");
			exit(EXIT_FAILURE);
		}
	}
	for (i = 0; i < n_buffers_; ++i) {
		COMMON_V4L2_CLEAR(buf_);
		buf_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf_.memory = V4L2_MEMORY_MMAP;
		buf_.index = i;
		xioctl(fd_, VIDIOC_QBUF, &buf_);
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	xioctl(fd_, VIDIOC_STREAMON, &type);
}

void common_v4l2::update_image(){
	fd_set fds;
	int r;
	struct timeval tv;

	do {
		FD_ZERO(&fds);
		FD_SET(fd_, &fds);

		/* Timeout. */
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		r = select(fd_ + 1, &fds, NULL, NULL, &tv);
	} while ((r == -1 && (errno == EINTR)));
	if (r == -1) {
		perror("select");
		exit(EXIT_FAILURE);
	}
	COMMON_V4L2_CLEAR(buf_);
	buf_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf_.memory = V4L2_MEMORY_MMAP;
	xioctl(fd_, VIDIOC_DQBUF, &buf_);

	shared_ptr<vector<char>> buf = make_shared<vector<char>>();
	buf->resize(buffers_[buf_.index].length);
	memcpy(&(*buf.get())[0], buffers_[buf_.index].start, buffers_[buf_.index].length);
	fetched_.push_back(buf);
	if(fetched_.size() >= size_buf_fetched_){
		fetched_.pop_front();
	}
	xioctl(fd_, VIDIOC_QBUF, &buf_);
}

void common_v4l2::deinit(){
	unsigned int i;
	enum v4l2_buf_type type;


	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	xioctl(fd_, VIDIOC_STREAMOFF, &type);
	for (i = 0; i < n_buffers_; ++i){
		v4l2_munmap(buffers_[i].start, buffers_[i].length);
	}
	v4l2_close(fd_);
	free(buffers_);

}

void common_v4l2::set_brightness(int v)
{
		v4l2_control c;
		c.id = V4L2_CID_BRIGHTNESS;
		c.value = v;
		xioctl(fd_, VIDIOC_S_CTRL, &c);
}

void common_v4l2::set_hue(int v)
{
		v4l2_control c;
		c.id = V4L2_CID_HUE;
		c.value = v;
		xioctl(fd_, VIDIOC_S_CTRL, &c);
}

void common_v4l2::set_colour(int v)
{
		v4l2_control c;
		c.id = V4L2_CID_SATURATION;
		c.value = v;
		xioctl(fd_, VIDIOC_S_CTRL, &c);
}

void common_v4l2::set_contrast(int v)
{
		v4l2_control c;
		c.id = V4L2_CID_CONTRAST;
		c.value = v;
		xioctl(fd_, VIDIOC_S_CTRL, &c);
}

void common_v4l2::set_exposure(int v)
{
	if(v >= 0){
		v4l2_control c;
		c.id = V4L2_CID_EXPOSURE_AUTO;
		c.value = V4L2_EXPOSURE_MANUAL;
		xioctl(fd_, VIDIOC_S_CTRL, &c);

		c.id = V4L2_CID_EXPOSURE_ABSOLUTE;
		c.value = v;
		xioctl(fd_, VIDIOC_S_CTRL, &c);
	}else{
		v4l2_control c;
		c.id = V4L2_CID_EXPOSURE_AUTO;
		c.value = V4L2_EXPOSURE_AUTO;
		xioctl(fd_, VIDIOC_S_CTRL, &c);
	}
}
