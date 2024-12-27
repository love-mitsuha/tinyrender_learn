#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>
#include "geometry.h"

//齐次式转换
col4fp point2homo(Vec3f v)
{
	col4fp m;
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}

col4fv vec2homo(Vec3f v)
{
	col4fp m;
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 0.f;
	return m;
}


//由归一化坐标系转换为视口坐标系
Matrix4f NDC2view(int x, int y, int w, int h, int depth)
{
	Matrix4f m = Matrix4f::identity(4);
	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;

	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;
	return m;
}

Vec3f perspective_homo2vec(col4fp m)
{
	Vec3f v;
	v.x = m[0][0] / m[3][0];
	v.y = m[1][0] / m[3][0];
	v.z = m[2][0] / m[3][0];
	return v;
}

Matrix4f View(Vec3f camera, Vec3f center, Vec3f up)
{
	Vec3f z = (camera - center).normalize();
	Vec3f x = cross(up, z).normalize();
	Vec3f y = cross(z, x).normalize();
	Matrix4f m = Matrix4f::identity(4);
	Matrix4f tr = Matrix4f::identity(4);
	for (int i = 0; i < 3; i++)
	{
		m[0][i] = x[i];
		m[1][i] = y[i];
		m[2][i] = z[i];
		tr[i][3] = -camera[i];
	}
	return m * tr;
}

Vec3f homo2vec(col4fv m)
{
	Vec3f v;
	v.x = m[0][0];
	v.y = m[1][0];
	v.z = m[2][0];
	return v;
}

Vec3f color2vec(TGAColor color)
{
	Vec3f v;
	v.x = color.r * 2.0 / 255.0 - 1.f;
	v.y = color.g * 2.0 / 255.0 - 1.f;
	v.z = color.b * 2.0 / 255.0 - 1.f;
	return v;
}


