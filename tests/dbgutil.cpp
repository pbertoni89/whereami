/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "dbgutil.h"

using namespace cv;

void dump_data(const struct quirc_data *data)
{
	printf("    Version: %d\n", data->version);
	printf("    ECC level: %c\n", "MLHQ"[data->ecc_level]);
	printf("    Mask: %d\n", data->mask);
	printf("    Data type: %d\n", data->data_type);
	printf("    Length: %d\n", data->payload_len);
	printf("    Payload: %s\n", data->payload);
}

void dump_cells(const struct quirc_code *code)
{
	int u, v;

	printf("    %d cells, corners:", code->size);
	for (u = 0; u < 4; u++)
		printf(" (%d,%d)", code->corners[u].x,
				   code->corners[u].y);
	printf("\n");

	for (v = 0; v < code->size; v++) {
		printf("    ");
		for (u = 0; u < code->size; u++) {
			int p = v * code->size + u;

			if (code->cell_bitmap[p >> 3] & (1 << (p & 7)))
				printf("[]");
			else
				printf("  ");
		}
		printf("\n");
	}
}

int load_image(struct quirc *q, const char *filename){
  Mat img = imread(filename,CV_LOAD_IMAGE_GRAYSCALE);
  printf("%d\n",img.channels());
  imshow("OpenCV image",img);
  cv_to_quirc(q, img);
  
  return 0;
}

void cv_to_quirc(struct quirc *q, Mat& img){
	uint8_t *image;
	quirc_resize(q, img.cols, img.rows);

	image = quirc_begin(q, NULL, NULL);
    
	for (int y = 0; y < img.rows; y++) {
    uint8_t *row_pointer = image + y * img.cols;
    for(int x = 0; x < img.cols; x++){
        row_pointer[x] = img.at<uint8_t>(x,y);
    }
	}
}
