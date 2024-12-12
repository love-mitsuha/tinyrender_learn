#include "Bresenham.h"
#include "geometry.h"
#include <cmath>

void openTrangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
	line_bre(p0.x, p0.y, p1.x, p1.y, image, color);
	line_bre(p1.x, p1.y, p2.x, p2.y, image, color);
	line_bre(p2.x, p2.y, p0.x, p0.y, image, color);
}

void filledTriangleScan(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
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

//计算中心坐标时一定要注意使用浮点 并且减少除法运算
Vec3f baryCentric(Vec3f p0, Vec3f p1, Vec3f p2, Vec3f p)
{
	//这一版可能因为除法太多精度缺失
	/*float ABx = p1.x - p0.x;
	float ACx = p2.x - p0.x;
	float PAx = p0.x - p.x;
	float ABy = p1.y - p0.y;
	float ACy = p2.y - p0.y;
	float PAy = p0.y - p.y;
	float v = (PAx * ABy / ABx - PAy) / (ACy - ACx * ABy / ABx);
	float u = -PAx / ABx - v * ACx / ABx;
	return { 1 - u - v,u,v };*/
	
	Vec3f s[2];
	s[0].x = p1.x - p0.x;//ABx
	s[0].y = p2.x - p0.x;//ACx
	s[0].z = p0.x - p.x;//PAx
	s[1].x = p1.y - p0.y;//ABy
	s[1].y = p2.y - p0.y;//ACy
	s[1].z = p0.y - p.y;//PAy
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u.z)>1e-3)
		return Vec3f(1.f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);//先转换成(u,v,1) 再返回(1-u-v,u,v)
	return Vec3f(-1, 1, 1);
}

Vec3f baryCentric(Vec2f p0, Vec2f p1, Vec2f p2, Vec2f p)
{
	//Vec3f s[2];
	//s[0].x = p1.x - p0.x;//ABx
	//s[0].y = p2.x - p0.x;//ACx
	//s[0].z = p0.x - p.x;//PAx
	//s[1].x = p1.y - p0.y;//ABy
	//s[1].y = p2.y - p0.y;//ACy
	//s[1].z = p0.y - p.y;//PAy
	//Vec3f u = cross(s[0], s[1]);
	//if (std::abs(u.z) > 1e-3)
	//	return Vec3f(1.f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);//先转换成(u,v,1) 再返回(1-u-v,u,v)
	//return Vec3f(-1, 1, 1);
	Vec3f s[2];
	for (int i = 0; i < 2; i++) {
		s[i].x = p1[i] - p0[i];
		s[i].y = p2[i] - p0[i];
		s[i].z = p0[i] - p[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u.z) > 1e-3)
		return Vec3f(1.f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
	return Vec3f(-1, 1, 1);
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