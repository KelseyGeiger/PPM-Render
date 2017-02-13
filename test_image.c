#include "ppm.h"
#include "draw.h"
#include "math.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int floor_i(float f)
{
	return (int)(f);
}

static int round_i(float f)
{
	return floor_i(f + 0.5f);
}

static float frac(float f)
{
	if(f < 0) {
		return 1.0f - (f - floor_i(f));
	}
	
	return f - floor_i(f);
}

static float remainder_frac(float f)
{
	return 1.0f - frac(f);
}

ColorRGB color_field(Point2D point);
ColorRGB heightmap(Point2D point);

float noise_2D(int x, int y);
float smooth_noise_2D(int x, int y);
float interpolated_noise(float x, float y);
float cos_interp(float x, float y, float a);
float Perlin2D(float x, float y, float pers, int n_oct);

int main(int argc, char** argv)
{	
	Image* image = ppm_create(1024, 1024);
	Image* smaller = ppm_create(128, 128);
	
	int x, y;
	for(y = 0; y < image->header.height; y++) {
		for(x = 0; x < image->header.width; x++) {
			Point2D pix = point2(x, y);
			float x_scale = image->header.width / 8.0f;
			float y_scale = image->header.height / 8.0f;
			Point2D samp = point2((pix.x + 512) / x_scale, (pix.y + 512) / y_scale);
			//ColorRGB color = color_field(samp);
			ColorRGB height = heightmap(samp);
			//ColorRGB blended = blend(color, height, 0.1f);
			draw_point(image, pix, height);
		}
	}

	for(y = 0; y < smaller->header.height; y++) {
		for(x = 0; x < image->header.width; x++) {
			Point2D pix = point2(x, y);
			float x_scale = image->header.width / 8.0f;
			float y_scale = image->header.height / 8.0f;
			Point2D sample = point2((pix.x) / x_scale, (pix.y) / y_scale);
			ColorRGB color = heightmap(sample);
			draw_point(smaller, pix, color);
		}
	}
	
	Rect2D src_rect, dest_rect;
	src_rect = (Rect2D) { point2(0, 0), point2(smaller->header.width, smaller->header.height) };
	dest_rect = (Rect2D) { point2(128, 128), point2(448, 448) };
	blit_alpha(image, dest_rect, smaller, src_rect, 0.1f);
	
	ppm_save(image, "overlay.ppm");

	ppm_destroy(smaller);
	ppm_destroy(image);
	
	return 0;
}

ColorRGB heightmap(Point2D point)
{
	static int init = 0;
	static ColorRGB greyscale[256];
	
	if(!init) {
		int i;
		for(i = 0; i < 256; i++) {
			greyscale[i] = rgb(i, i, i);
		}
		init = 1;
	}
	
	float color_percent = Perlin2D(point.x, point.y, 0.5, 10);
	
	if(color_percent < 0) {
		color_percent = 0.0f;
	}
	
	if(color_percent > 1) {
		color_percent = 1.0f;
	}
	
	color_percent *= 255;
	
	int rounded = round_i(color_percent);
	
	return greyscale[rounded];
}

ColorRGB color_field(Point2D point)
{
	ColorRGB colors[] = {
		rgb(0, 0, 64),
		rgb(128, 0, 128),
		rgb(128, 0, 255),
		rgb(128, 64, 255),
		rgb(64, 64, 255),
		rgb(0, 128, 255),
		rgb(0, 192, 255),
		rgb(0, 255, 255),
		rgb(0, 255, 128),
		rgb(0, 255, 64),
		rgb(64, 255, 64),
		rgb(64, 255, 0),
		rgb(128, 255, 0),
		rgb(255, 255, 0),
		rgb(255, 128, 0),
		rgb(128, 0, 0),
		rgb(255, 0, 0),
		rgb(255, 64, 64),
		rgb(255, 128, 128),
		rgb(255, 192, 192),
		rgb(255, 255, 255)
	};
	
	float color_percent = Perlin2D(point.x, point.y, 0.5, 10);
	
	if(color_percent < 0) {
		color_percent = 0.0f;
	}
	
	if(color_percent > 1) {
		color_percent = 1.0f;
	}
	
	color_percent *= ((sizeof(colors) / sizeof(colors[0])) - 1);
	
	int rounded = round_i(color_percent);
	float alpha = frac(color_percent);
	
	int bg, fg;
	
	if(alpha > 0.5) {
		bg = rounded - 1;
		fg = rounded;
		alpha = 1.0f - alpha;
	} else {
		bg = rounded;
		fg = rounded + 1;
	}
	
	return blend(colors[bg], colors[fg], alpha);
}

float noise_2D(int x, int y)
{
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return 1.0f - ( ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0f);
}

float smooth_noise_2D(int x, int y)
{
	int x0 = x-1;
	int x1 = x+1;
	int y0 = y-1;
	int y1 = y+1;
	
	float corners = (noise_2D(x0, y0) + noise_2D(x1, y0) + noise_2D(x0, y1) + noise_2D(x1, y1)) / 16.0f;
	float sides = (noise_2D(x0, y) + noise_2D(x1, y) + noise_2D(x, y0) + noise_2D(x, y1)) / 8.0f;
	float center = noise_2D(x, y);
	
	return center + sides + corners;
}

float interpolated_noise(float x, float y)
{
	int x_ = floor_i(x);
	int y_ = floor_i(y);
	
	float frac_x = frac(x);
	float frac_y = frac(y);
	
	float v1 = smooth_noise_2D(x_, y_);
	float v2 = smooth_noise_2D(x_ + 1, y_);
	float v3 = smooth_noise_2D(x_, y_ + 1);
	float v4 = smooth_noise_2D(x_ + 1, y_ + 1);
	
	float i1 = cos_interp(v1, v2, frac_x);
	float i2 = cos_interp(v3, v4, frac_x);
	
	return cos_interp(i1, i2, frac_y);
}

float cos_interp(float a, float b, float t)
{
	float theta = t * M_PI;
	float f = (1.0f - cos(theta)) * 0.5f;

	return (1.0f - f) * a + f * b;
}

float Perlin2D(float x, float y, float persistence, int n_octave)
{
	float total = 0;
	float p = persistence;
	
	int i;
	for(i = 0; i < n_octave; i++) {
		
		float freq = pow(2, i);
		float ampl = pow(p, i);
		
		total += interpolated_noise(x * freq, y * freq) * ampl;
	}
	
	return total;
}
