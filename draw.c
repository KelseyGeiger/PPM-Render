#include "draw.h"

#define abs(x) ( ((x) < 0) ? -(x) : (x))

static void swap(float* l, float* r)
{
	float tmp = *l;
	*l = *r;
	*r = tmp;
}

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

Rect2D tri_bounds(Triangle2D tri) {
	Point2D min, max;
	min = tri.p1;
	max = tri.p1;
	
	if(tri.p2.x < min.x) {
		min.x = tri.p2.x;
	} else if(tri.p2.x > max.x) {
		max.x = tri.p2.x;
	}
	
	if(tri.p3.x < min.x) {
		min.x = tri.p3.x;
	} else if(tri.p3.x > max.x) {
		max.x = tri.p3.x;
	}
	
	if(tri.p2.y < min.y) {
		min.y = tri.p2.y;
	} else if(tri.p2.y > max.y) {
		max.y = tri.p2.y;
	}
	
	if(tri.p3.y < min.y) {
		min.y = tri.p3.y;
	} else if(tri.p3.y > max.y) {
		max.y = tri.p3.y;
	}
	
	return (Rect2D) { min, max };
}

Vec3D barycentric_coords(Triangle2D tri, Point2D point)
{
	Vec3D v = vec3(tri.p3.x - tri.p1.x, tri.p2.x - tri.p1.x, tri.p1.x - point.x);
	Vec3D u = vec3(tri.p3.y - tri.p1.y, tri.p2.y - tri.p1.y, tri.p1.y - point.y);
	
	Vec3D r = vec3_cross(v, u);
	
	if(abs(r.y) < 1) return vec3(-1, 1, 1);
	
	return vec3(1 - (r.x + r.y)/r.z, r.y/r.z, r.x/r.z);
}

Vec2D vec2(float x, float y)
{
	return (Vec2D) { x, y };
}

Vec3D vec3(float x, float y, float z)
{
	return (Vec3D) { x, y, z };
}

Point2D point2(float x, float y)
{
	return (Point2D) { x, y };
}

ColorRGB rgb(int r, int g, int b)
{
	return (ColorRGB) { 
		(uint8_t) clamp(r, 0, 255),
		(uint8_t) clamp(g, 0, 255),
		(uint8_t) clamp(b, 0, 255)
	};
}

Vec2D vec2_add(Vec2D l, Vec2D r)
{
	return (Vec2D) { l.x + r.x, l.y + r.y };
}

Vec2D vec2_sub(Vec2D l, Vec2D r)
{
	return (Vec2D) { l.x - r.x, l.y - r.y };
}

Vec2D vec2_scale(Vec2D v, float scale)
{
	return (Vec2D) { v.x * scale, v.y * scale };
}

float vec2_dot(Vec2D l, Vec2D r)
{
	return (l.x * r.x) + (l.y * r.y);
}

float vec2_mag(Vec2D v)
{
	return sqrt(vec2_dot(v, v));
}

float vec2_mag_sqr(Vec2D v)
{
	return vec2_dot(v, v);
}

Vec2D vec2_negate(Vec2D v)
{
	return (Vec2D) { -v.x, -v.y };
}

Vec3D vec3_add(Vec3D l, Vec3D r)
{
	return (Vec3D) { l.x + r.x, l.y + r.y, l.z + r.z };
}

Vec3D vec3_sub(Vec3D l, Vec3D r)
{
	return (Vec3D) { l.x - r.x, l.y - r.y, l.z - r.z };
}

Vec3D vec3_scale(Vec3D v, float scale)
{
	return (Vec3D) { v.x * scale, v.y * scale, v.z * scale };
}

float vec3_dot(Vec3D l, Vec3D r)
{
	return (l.x * r.x) + (l.y * r.y) + (l.z * r.z);
}

Vec3D vec3_cross(Vec3D l, Vec3D r)
{
	float vx = (l.y * r.z) - (l.z * r.y);
	float vy = -((l.x * r.z) - (l.z * r.x));
	float vz = (l.x * r.y) - (l.y * r.x);
	
	return (Vec3D) { vx, vy, vz };
}

float vec3_mag(Vec3D v)
{
	return sqrt(vec3_dot(v, v));
}

float vec3_mag_sqr(Vec3D v)
{
	return vec3_dot(v, v);
}

Vec3D vec3_negate(Vec3D v)
{
	return (Vec3D) { -v.x, -v.y, -v.z };
}

ColorRGB blend(ColorRGB bg, ColorRGB fg, float alpha)
{
	ColorRGB blended;
	blended.r = (uint8_t)((alpha * fg.r) + ((1.0f - alpha) * bg.r));
	blended.g = (uint8_t)((alpha * fg.g) + ((1.0f - alpha) * bg.g));
	blended.b = (uint8_t)((alpha * fg.b) + ((1.0f - alpha) * bg.b));
	
	return blended;
}

void draw_point(Image* image, Point2D point, ColorRGB color)
{
	ppm_set_pixel(image, round(point.x), round(image->header.height - point.y), color);
}

void draw_point_alpha(Image* image, Point2D point, ColorRGB color, float alpha)
{
	ColorRGB blended = blend(ppm_get_pixel(image, round(point.x), round(point.y)), color, alpha);
	ppm_set_pixel(image, round(point.x), round(image->header.height - point.y), blended);
}

void draw_line(Image* image, Point2D p1, Point2D p2, ColorRGB color)
{
	bool steep = false;

	if(abs(p1.x-p2.x) < abs(p1.y-p2.y)) {
		swap(&p1.x, &p1.y);
		swap(&p2.x, &p2.y);
		steep = true;
	}
	
	if(p1.x > p2.x) {
		swap(&p1.x, &p2.x);
		swap(&p1.y, &p2.y);
	}
	
	int dx = round_i(p2.x - p1.x);
	int dy = round_i(p2.y - p1.y);
	int derr = abs(dy) * 2;
	int err = 0;
	int y = round_i(p1.y);
	int x = round_i(p1.x);
	
	for(; x < round_i(p2.x); x++) {
		if(steep) {
			draw_point(image, point2(y, x), color);
		} else {
			draw_point(image, point2(x, y), color);
		}
		
		err += derr;
		if(err > dx) {
			y += (p2.y > p1.y) ? 1 : -1;
			err -= dx * 2;
		}
	}
}

void draw_line_alpha(Image* image, Point2D p1, Point2D p2, ColorRGB color, float alpha)
{
	bool steep = false;

	if(abs(p1.x-p2.x) < abs(p1.y-p2.y)) {
		swap(&p1.x, &p1.y);
		swap(&p2.x, &p2.y);
		steep = true;
	}
	
	if(p1.x > p2.x) {
		swap(&p1.x, &p2.x);
		swap(&p1.y, &p2.y);
	}
	
	int dx = round_i(p2.x - p1.x);
	int dy = round_i(p2.y - p1.y);
	int derr = abs(dy) * 2;
	int err = 0;
	int y = round_i(p1.y);
	int x = round_i(p1.x);
	
	for(; x < round_i(p2.x); x++) {
		if(steep) {
			draw_point_alpha(image, point2(y, x), color, alpha);
		} else {
			draw_point_alpha(image, point2(x, y), color, alpha);
		}
		
		err += derr;
		if(err > dx) {
			y += (p2.y > p1.y) ? 1 : -1;
			err -= dx * 2;
		}
	}
}

void draw_circle(Image* image, Point2D origin, float radius, ColorRGB color, bool filled)
{
	if(filled) {
		int x;
		int y;
		
		for(y = -round(radius); y < round(radius); y++) {
			for(x = -round(radius); x < round(radius); x++) {
				if((x * x) + (y * y) < (radius * radius)) {
					draw_point(image, point2(x + origin.x, y + origin.y), color);
				}
			}
		}
	} else {
		int x = round(radius);
		int y = 0;
		int decision = 1 - x;
		
		while(y <= x) {
			draw_point(image, point2(origin.x + x, origin.y + y), color);
			draw_point(image, point2(origin.x + y, origin.y + x), color);
			draw_point(image, point2(origin.x - x, origin.y + y), color);
			draw_point(image, point2(origin.x - y, origin.y + x), color);
			draw_point(image, point2(origin.x - x, origin.y - y), color);
			draw_point(image, point2(origin.x - y, origin.y - x), color);
			draw_point(image, point2(origin.x + x, origin.y - y), color);
			draw_point(image, point2(origin.x + y, origin.y - x), color);
			
			y++;
			
			if(decision <= 0) {
				decision += (2 * y) + 1;
			} else {
				x--;
				decision += 2 * (y - x) + 1;
			}
		}
	}
}

void draw_circle_alpha(Image* image, Point2D origin, float radius, ColorRGB color, float alpha, bool filled)
{
	if(filled) {
		int x = round(origin.x - radius);
		int y = round((image->header.height - origin.y) - radius);
		
		for(; y < origin.y + radius; y++) {
			for(; x < origin.x + radius; x++) {
				if((x * x) + (y * y) < (radius * radius)) {
					draw_point_alpha(image, point2(x, image->header.height - y), color, alpha);
				}
			}
		}
	} else {
		int x = round(radius);
		int y = 0;
		int decision = 1 - x;
		
		while(y <= x) {
			draw_point_alpha(image, point2(origin.x + x, origin.y + y), color, alpha);
			draw_point_alpha(image, point2(origin.x + y, origin.y + x), color, alpha);
			draw_point_alpha(image, point2(origin.x - x, origin.y + y), color, alpha);
			draw_point_alpha(image, point2(origin.x - y, origin.y + x), color, alpha);
			draw_point_alpha(image, point2(origin.x - x, origin.y - y), color, alpha);
			draw_point_alpha(image, point2(origin.x - y, origin.y - x), color, alpha);
			draw_point_alpha(image, point2(origin.x + x, origin.y - y), color, alpha);
			draw_point_alpha(image, point2(origin.x + y, origin.y - x), color, alpha);
			
			y++;
			
			if(decision <= 0) {
				decision += (2 * y) + 1;
			} else {
				x--;
				decision += 2 * (y - x) + 1;
			}
		}
	}
}

void draw_triangle(Image* image, Triangle2D tri, ColorRGB color, bool filled)
{
	if(!filled) {
		draw_line(image, tri.p1, tri.p2, color);
		draw_line(image, tri.p2, tri.p3, color);
		draw_line(image, tri.p3, tri.p1, color);
	} else {
		Rect2D bounds = tri_bounds(tri);
		int x;
		int y;
		
		for(y = round(bounds.top_left.y) - 1; y <= round(bounds.bot_right.y); y++) {
			for(x = round(bounds.top_left.x) - 1; x <= round(bounds.bot_right.x); x++) {
				Vec3D bary = barycentric_coords(tri, point2(x, y));
				
				if(bary.x < 0 || bary.y < 0 || bary.z < 0) {
					continue;
				} else {
					draw_point(image, point2(x, y), color);
				}
			}
		}
	}
}

void draw_triangle_alpha(Image* image, Triangle2D tri, ColorRGB color, float alpha, bool filled)
{
	if(!filled) {
		draw_line_alpha(image, tri.p1, tri.p2, color, alpha);
		draw_line_alpha(image, tri.p2, tri.p3, color, alpha);
		draw_line_alpha(image, tri.p3, tri.p1, color, alpha);
	} else {
		Rect2D bounds = tri_bounds(tri);
		int x;
		int y;
		
		for(y = round(bounds.top_left.y) - 1; y <= round(bounds.bot_right.y); y++) {
			for(x = round(bounds.top_left.x) - 1; x <= round(bounds.bot_right.x); x++) {
				Vec3D bary = barycentric_coords(tri, point2(x, y));
				
				if(bary.x < 0 || bary.y < 0 || bary.z < 0) {
					continue;
				} else {
					draw_point_alpha(image, point2(x, y), color, alpha);
				}
			}
		}
	}
}

void blit(Image* dest, Rect2D dest_rect, const Image* src, Rect2D src_rect)
{
	unsigned int dest_x, dest_y;
	unsigned int src_x, src_y;
	unsigned dest_width, dest_height;
	unsigned src_width, src_height;
	
	dest_width = dest_rect.top_right.x - dest_rect.bottom_left.x;
	dest_height = dest_rext.top_right.y - dest_rect.bottom_left.y;
	
	src_width = src_rect.top_right.x - src_rect.bottom_left.x;
	src_height = src_rect.top_right.y - src_rect.bottom_left.y;
	
	dest_x = dest_rect.bottom_left.x;
	dest_y = dest_rect.top_right.y;
	
	src_x = src_rect.bottom_left.x;
	src_y = src_rect.top_right.y;
	
	float tx = 0;
	float ty = 0;
	
	for(; dest_y < dest_rect.bottom_left.y; dest_y--) {
		ty = (float)(dest_y - dest_rect.bottom_left.y)/dest_height;
		src_y = src_rect.top_right.y - round_i(ty * src_height);		
		
		if(src_y > src->height) {
			continue;
		}
		
		for(; dest_x < dest_rect.top_right.x; dest_x++) {
			tx = (float)(dest_x - dest_rect.bottom_left.x)/dest_width;
			src_x = src_rect.bottom_left.x + round_i(tx * src_width);
			
			if(src_x < src->width) {
				ColorRGB color = ppm_get_pixel(src, src_x, src->height - src_y);
				draw_point(dest, point2(dest_x, dest_y), color);
			}
		}
	}
}

void blit_alpha(Image* dest, Rect2D dest_rect, const Image* src, Rect2D src_rect, float alpha)
{

}
