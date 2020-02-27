#ifndef __U_JPEG_DECODER__
#define __U_JPEG_DECODER__

#include <vector>

class jpeg_decoder {
public:
	static int decode(unsigned char* out, int size_buf_out, const unsigned char* in, int size_in);
};

#endif
