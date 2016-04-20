#ifndef PPM_H
#define PPM_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	int width;
	int height;
} PPM_Header;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} PPM_Pixel;

typedef struct {
	PPM_Header header;
	PPM_Pixel* buffer;
} PPM_Image;

PPM_Pixel ppm_rgb(int r, int g, int b);

PPM_Image* ppm_create(int w, int h);
void ppm_destroy(PPM_Image* img);

void ppm_set_pixel(PPM_Image* img, int x, int y, PPM_Pixel pixel);
void ppm_set_rgb(PPM_Image* img, int x, int y, int r, int g, int b);
PPM_Pixel ppm_get_pixel(PPM_Image* img, int x, int y);

void ppm_save(PPM_Image* img, const char* filename);
PPM_Image* ppm_load(const char* filename);

#endif //PPM_H
