#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>
#include "geometry.h"

//齐次式转换
col4f point2homo(Vec3f v)
{
	col4f m;
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}

Vec3f homo2point(col4f m)
{
	Vec3f v;
	v.x = m[0][0];
	v.y = m[1][0];
	v.z = m[2][0];
	return v;
}

//由归一化坐标系转换为视口坐标系
Matrix4f NDC2view(int x, int y, int w, int h)
{
	Matrix4f m = Matrix4f::identity(3);
	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;

	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	return m;
}

Vec3f perspective_homo2point(col4f m)
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

Vec3f homo2vec(col4f m)
{
	Vec3f v;
	v.x = m[0][0];
	v.y = m[1][0];
	v.z = m[2][0];
	return v;
}



