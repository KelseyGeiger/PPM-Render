#ifndef DRAW_H
#define DRAW_H

#include "ppm.h"
#include <math.h>
#include <stdbool.h>

typedef struct {
	float x;
	float y;
} Vec2D;

typedef struct {
	float x;
	float y;
	float z;
} Vec3D;

typedef Vec2D Point2D;
typedef Vec3D Point3D;
typedef PPM_Pixel ColorRGB;
typedef PPM_Image Image;

typedef struct {
	Point2D bot_left;
	Point2D top_right;
} Rect2D;

typedef struct {
	Point2D p1;
	Point2D p2;
	Point2D p3;
} Triangle2D;

Vec2D vec2(float x, float y);
Vec3D vec3(float x, float y, float z);
Point2D point2(float x, float y);
ColorRGB rgb(int r, int g, int b);

Vec2D vec2_add(Vec2D l, Vec2D r);
Vec2D vec2_sub(Vec2D l, Vec2D r);
Vec2D vec2_scale(Vec2D v, float scale);
float vec2_dot(Vec2D l, Vec2D r);
float vec2_mag(Vec2D v);
float vec2_mag_sqr(Vec2D v);
Vec2D vec2_negate(Vec2D v);

Vec3D vec3_add(Vec3D l, Vec3D r);
Vec3D vec3_sub(Vec3D l, Vec3D r);
Vec3D vec3_scale(Vec3D v, float scale);
float vec3_dot(Vec3D l, Vec3D r);
Vec3D vec3_cross(Vec3D l, Vec3D r);
float vec3_mag(Vec3D v);
float vec3_mag_sqr(Vec3D v);
Vec3D vec3_negate(Vec3D v);

ColorRGB blend(ColorRGB bg, ColorRGB fg, float alpha);

Vec3D barycentric_coords(Triangle2D tri, Point2D point);

void draw_point(Image* image, Point2D point, ColorRGB color);
void draw_point_alpha(Image* image, Point2D point, ColorRGB color, float alpha);

void draw_line(Image* image, Point2D p1, Point2D p2, ColorRGB color);
void draw_line_alpha(Image* image, Point2D p1, Point2D p2, ColorRGB color, float alpha);

void draw_circle(Image* image, Point2D origin, float radius, ColorRGB color, bool filled);
void draw_circle_alpha(Image* image, Point2D origin, float radius, ColorRGB color, float alpha, bool filled);

void draw_triangle(Image* image, Triangle2D tri, ColorRGB color, bool filled);
void draw_triangle_alpha(Image* image, Triangle2D tri, ColorRGB color, float alpha, bool filled);

void blit(Image* dest, Rect2D dest_rect, const Image* src, Rect2D src_rect);
void blit_alpha(Image* dest, Rect2D dest_rect, const Image* src, Rect2D src_rect, float alpha);

#endif //DRAW_H
