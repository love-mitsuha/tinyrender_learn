#include "tgaimage.h"
#include "Bresenham.h"

void line_fir(int x1, int y1, int x2, int y2, TGAImage& image, TGAColor color)
{
	for (float t = 0.; t < 1.; t+=.0001)
	{
		int x = x1 + (x2 - x1) * t;
		int y = y1 + (y2 - y1) * t;
		image.set(x, y, color);
	}
}

void line_sec(int x1, int y1, int x2, int y2, TGAImage& image, TGAColor color)
{
	for (int x = x1; x <= x2; x++)
	{
		float t = (x - x1) / (float)(x2 - x);
		int y = (t * y2 + y1) / (t + 1);
		image.set(x, y, color);
	}
}
void line_sec_test(int x1, int y1, int x2, int y2, TGAImage& image, TGAColor color)
{
	for (int x = x1; x < x2; x++)
	{
		float t = (x - x1) / (float)(x2 - x1);
		int y = y1 * (1 - t) + y2 * t;
		image.set(x, y, color);
	}
}
void line_thir(int x1, int y1, int x2, int y2, TGAImage& image, TGAColor color)
{
	bool swap = false;
	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	if (std::abs(x2 - x1) < std::abs(y2 - y1))
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
		swap = true;
	}
	for (int x = x1; x <= x2; x++)
	{
		float t = (x - x1) / (float)(x2 - x);
		int y = (t * y2 + y1) / (t + 1);
		if (swap)
			image.set(y, x, color);
		else
			image.set(x, y, color);
	}
}
void line_four(int x1, int y1, int x2, int y2, TGAImage& image, TGAColor color)
{
	bool swap = false;
	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	if (std::abs(x2 - x1) < std::abs(y2 - y1))
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
		swap = true;
	}
	float k = std::abs((float)(y2 - y1) / (x2 - x1));
	float error = 0.;
	int y = y1;
	for (int x = x1; x <= x2; x++)
	{
		if (swap)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
		error += k;
		if (error>0.5)
		{
			y += y2>y1?1:-1;
			error -= 1;
		}
		
	}
}
void line_bre(int x1, int y1, int x2, int y2, TGAImage& image, TGAColor color)
{
	bool swap = false;
	if (std::abs(x2 - x1) < std::abs(y2 - y1))//必须先比较斜率再比较x1和x2正负
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
		swap = true;
	}
	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	int derror = std::abs(dy * 2);
	int error = 0;
	int y = y1;
	for (int x = x1; x <= x2; x++)
	{
		if (swap)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
		error += derror;
		if (error>dx)
		{
			y += y2 > y1 ? 1 : -1;//斜率为正还是负
			error -= 2 * dx;
		}
	}
}
