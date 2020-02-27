#include "jpeg_decoder.h"
#include <stdio.h>
#include <jpeglib.h>

int jpeg_decoder::decode(unsigned char* out, int size_buf_out, const unsigned char* in, int size_in)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, in, size_in);
	int rc = jpeg_read_header(&cinfo, TRUE);
	if(rc != 1){
		return -1;
	}
	jpeg_start_decompress(&cinfo);
	int size = cinfo.output_width * cinfo.output_height * cinfo.output_components;	if(size_buf_out < size){
		return -2;
	}
	int row_stride = cinfo.output_width * cinfo.output_components;
	while(cinfo.output_scanline < cinfo.output_height){
		unsigned char* buffer_array[1];
		buffer_array[0] = out + cinfo.output_scanline * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 4);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
}
