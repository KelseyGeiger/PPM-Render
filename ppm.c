#include "ppm.h"

int min(int l, int r)
{
	return (l < r) ? l : r;
}

int max(int l, int r)
{
	return (l > r) ? l : r;
}

int clamp(int in, int less, int greater)
{
	return min(in, max(greater, in));
}

PPM_Pixel ppm_rgb(int r, int g, int b)
{
	r = clamp(r, 0, 255);
	g = clamp(g, 0, 255);
	b = clamp(b, 0, 255);
	
	return (PPM_Pixel) { (uint8_t)(r), (uint8_t)(g), (uint8_t)(b) };
}

PPM_Image* ppm_create(int w, int h)
{
	PPM_Image* img;
	
	img = malloc(sizeof(PPM_Image));
	
	if(!img) {
		fprintf(stderr, "Error: failed to allocate PPM image.\n");
		exit(EXIT_FAILURE);
	}
	
	img->header.width = w;
	img->header.height = h;
	img->buffer = malloc(sizeof(PPM_Pixel) * img->header.width * img->header.height);
	
	if(!img->buffer) {
		fprintf(stderr, "Error: failed to allocate PPM image buffer.\n");
		free(img);
		exit(EXIT_FAILURE);
	}
	
	int i;
	for(i = 0; i < img->header.width * img->header.height; i++) {
		img->buffer[i] = (PPM_Pixel) {0, 0, 0};
	}
	
	return img;
}

void ppm_destroy(PPM_Image* img)
{
	if(img) {
		free(img->buffer);
		img->buffer = NULL;
	}
	free(img);
}

void ppm_set_pixel(PPM_Image* img, int x, int y, PPM_Pixel pixel)
{
	if(y < img->header.height && x < img->header.width && y >= 0 && x >= 0) {
		img->buffer[(y * img->header.width) + x] = pixel;
	}
}

void ppm_set_rgb(PPM_Image* img, int x, int y, int r, int g, int b)
{
	ppm_set_pixel(img, x, y, ppm_rgb(r, g, b));
}

PPM_Pixel ppm_get_pixel(PPM_Image* img, int x, int y)
{
	if(y < img->header.height && x < img->header.width && y >= 0 && x >= 0) {
		return img->buffer[(y * img->header.width) + x];
	}
	
	fprintf(stderr, "Error: out of bounds access in ppm_get_pixel.\n");
	exit(EXIT_FAILURE);
}

void ppm_save(PPM_Image* img, const char* filename)
{
	FILE* out;
	out = fopen(filename, "wb");
	
	if(!out) {
		fprintf(stderr, "Error opening output file.\n");
		exit(EXIT_FAILURE);
	}
	
	fprintf(out, "P6\n%d %d\n255\n", img->header.width, img->header.height);
	
	int i, j;
	for(j = 0; j < img->header.height; j++) {
		for(i = 0; i < img->header.width; i++) {
			uint8_t* buf = (uint8_t*)(&img->buffer[(j * img->header.width) + i]);
			fwrite(buf, 1, 3, out);
		}
	}
	fflush(out);
	fclose(out);
}

PPM_Image* ppm_load(const char* filename)
{
	FILE* in;
	in = fopen(filename, "rb");
	
	if(!in) {
		fprintf(stderr, "Error opening input file.\n");
		return;
	}
	
	char buf[2];
	int width, height;
	int max_val;
	
	fread(buf, 1, 2, in);
	
	if(buf[0] != 'P' || buf[1] != '6') {
		fprintf(stderr, "Error: unsupported format.\n");
		exit(EXIT_FAILURE);
	}
	
	while(fscanf(in, "%d %d", &width, &height) != 2);
	fscanf(in, "%d", &max_val);
	
	PPM_Image* img = ppm_create(width, height);
	
	int i, j;
	for(j = 0; j < img->header.height; j++) {
		for(i = 0; i < img->header.width; i++) {
			uint8_t components[3];
			fread(components, 1, 3, in);
			
			components[0] = 255 * components[0] / max_val;
			components[1] = 255 * components[1] / max_val;
			components[2] = 255 * components[2] / max_val;
			
			img->buffer[(j * img->header.width) + i] = (PPM_Pixel) { 
				components[0], 
				components[1], 
				components[2] 
			};
		}
	}
	
	fclose(in);
	
	return img;
}
