#include "common_v4l2.h"

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

void common_v4l2::init(char *dev_name, unsigned int x_res, unsigned int y_res) {
	enum v4l2_buf_type type;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_streamparm parm;
	parm.parm.capture.timeperframe.numerator = 1;
	parm.parm.capture.timeperframe.denominator = 260;
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
	fmt.fmt.pix.field	   = V4L2_FIELD_INTERLACED;
	xioctl(fd_, VIDIOC_S_FMT, &fmt);
	if ((fmt.fmt.pix.width != x_res) || (fmt.fmt.pix.height != y_res))
		printf("Warning: driver is sending image at %dx%d\n",
			fmt.fmt.pix.width, fmt.fmt.pix.height);
	COMMON_V4L2_CLEAR(req);
	req.count = 1;
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
	xioctl(fd_, VIDIOC_QBUF, &buf_);
}

char* common_v4l2::get_image(){
	return ((char *)buffers_[buf_.index].start);
}

size_t common_v4l2::get_image_size(){
	return buffers_[buf_.index].length;
}

void common_v4l2::deinit(){
	unsigned int i;
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	xioctl(fd_, VIDIOC_STREAMOFF, &type);
	for (i = 0; i < n_buffers_; ++i)
		v4l2_munmap(buffers_[i].start, buffers_[i].length);
	v4l2_close(fd_);
	free(buffers_);
}
