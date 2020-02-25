#include <stdio.h>
#include <stdlib.h>

#include "common_v4l2.h"

static void save_ppm(
	unsigned int i,
	unsigned int x_res,
	unsigned int y_res,
	size_t data_lenght,
	char *data
) {
	FILE *fout;
	char out_name[256];

	sprintf(out_name, "./out/%03d.jpg", i);
	fout = fopen(out_name, "w");
	if (!fout) {
		perror("error: fopen");
		exit(EXIT_FAILURE);
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
	v4.deinit();
	return EXIT_SUCCESS;
}
