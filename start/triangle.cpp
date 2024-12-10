#include "Bresenham.h"
#include "geometry.h"
#include <cmath>

void openTrangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
	line_bre(p0.x, p0.y, p1.x, p1.y, image, color);
	line_bre(p1.x, p1.y, p2.x, p2.y, image, color);
	line_bre(p2.x, p2.y, p0.x, p0.y, image, color);
}

void filledTrangleScan(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
	if (p0.y > p1.y)
		std::swap(p0, p1);
	if (p1.y > p2.y)
		std::swap(p1, p2);
	if (p0.y > p1.y)
		std::swap(p0, p1);
	float t = (p1.y - p0.y) / (float)(p2.y - p0.y);
	int x_split = round(t * (p2.x - p0.x) + p0.x);
	int x0 = p0.x;
	int y0 = p0.y;
	int x1 = p1.x;
	int y1 = p1.y;
	int x2 = p2.x;
	int y2 = p2.y;
	if (x_split>x1)
	{
		std::swap(x_split, x1);
	}
	float kp0 = (y0 - y1) / (float)(x0 - x_split);
	float k10 = (y0 - y1) / (float)(x0 - x1);
	float kp2 = (y2 - y1) / (float)(x2 - x_split);
	float k12 = (y2 - y1) / (float)(x2 - x1);
	for (int y = y0; y <= y1; y++)
	{
		int t1 = round((y - y0) / kp0 + x0);
		int t2 = round((y - y0) / k10 + x0);
		line_hori(t1, t2, y, image, color);
	}
	for (int  y = y1; y <= y2; y++)
	{
		int t1 = round((y - y2) / kp2 + x2);
		int t2 = round((y - y2) / k12 + x2);
		line_hori(t1, t2, y, image, color);
	}

}

Vec3f baryCentric(Vec2i p0, Vec2i p1, Vec2i p2, Vec2i p, TGAImage& image, TGAColor color)
{
	float ABx = p1.x - p0.x;
	float ACx = p2.x - p0.x;
	float PAx = p0.x - p.x;
	float ABy = p1.y - p0.y;
	float ACy = p2.y - p0.y;
	float PAy = p0.y - p.y;
	float v = (PAx * ABy / ABx - PAy) / (ACy - ACx * ABy / ABx);
	float u = -PAx / ABx - v * ACx / ABx;
	return { u,v,1-u-v };
}


void filledTrangleBary(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
	int xmin = std::min(p0.x, std::min(p1.x, p2.x));
	int xmax = std::max(p0.x, std::max(p1.x, p2.x));
	int ymin = std::min(p0.y, std::min(p1.y, p2.y));
	int ymax = std::max(p0.y, std::max(p1.y, p2.y));
	for (int x = xmin; x <= xmax; x++)
	{
		for (int y = ymin; y <= ymax; y++)
		{
			Vec3f BaryCentric = baryCentric(p0, p1, p2, Vec2i(x, y), image, color);
			if (BaryCentric.x >= 0 && BaryCentric.y >= 0 && BaryCentric.z >= 0)
			{
				image.set(x, y, color);
			}
		}
	}
}




//TODO
void filledTrangle_TODO(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
	if (p0.y>p1.y)
	{
		std::swap(p0.x, p1.x);
		std::swap(p0.y, p1.y);
	}
	if (p1.y > p2.y)
	{
		std::swap(p1.x, p2.x);
		std::swap(p1.y, p2.y);
	}
	if (p0.y > p1.y)
	{
		std::swap(p0.x, p1.x);
		std::swap(p0.y, p1.y);
	}

	float k = (p1.y - p0.y) / (float)(p2.y - p0.y);
	int x_split = round(k * (p2.x - p0.x) + p0.x);
	//(p0.x,p0.y) (x_split,p1.y) (p1.x,p1.y)
	bool swap_left = false;
	bool swap_right = false;
	int x_left, y_left, x_right, y_right;
	if (x_split > p1.x)
	{
		x_left = p1.x;
		y_left = p1.y;
		x_right = x_split;
		y_right = p1.y;
	}
	else
	{
		x_left = x_split;
		y_left = p1.y;
		x_right = p1.x;
		y_right = p1.y;
	}
	int x_left_down = p0.x;
	int y_left_down = p0.y;
	int x_right_down = p0.x;
	int y_right_down = p0.y;
	if (std::abs(x_left-x_left_down)>std::abs(y_left-y_left_down))
	{
		std::swap(x_left, y_left);
		std::swap(y_left_down, y_left_down);
		swap_left = true;
	}
	if (std::abs(x_right-x_right_down)>std::abs(y_right-y_right_down))
	{
		std::swap(x_right, y_right);
		std::swap(x_right_down, y_right_down);
		swap_right = true;
	}
	/*if (x_left>x_left_down)
	{
		std::swap(x_left, x_left_down);
		std::swap(y_left, y_left_down);
	}
	if (x_right_down>x_right)
	{
		std::swap(x_right, x_right_down);
		std::swap(y_right, y_right_down);
	}*/
	int dx_left = x_left_down - x_left;
	int dy_left = y_left_down - y_left;
	int dx_right = x_right - x_right_down;
	int dy_right = y_right - y_right_down;
	int derror_left = std::abs(2 * dy_left);
	int derror_right = std::abs(2 * dy_right);
	int error_left = 0;
	int error_right = 0;
	int x1 = x_left_down;
	int x2 = x_right_down;
	for (int y = p0.y; y < p1.y; y++)
	{

	}


}