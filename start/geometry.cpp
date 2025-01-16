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
	Matrix4f m = Matrix4f::identity(4);
	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;

	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	return m;
}

col4f perspective_div(col4f m)
{
	col4f v;
	v[0][0] = m[0][0] / m[3][0];
	v[1][0] = m[1][0] / m[3][0];
	v[2][0] = m[2][0] / m[3][0];
	v[3][0] = 1.f;
	return v;
}

// 相机与物体的相对运动 相机动则物体一并运动 相机平移至原点,朝向-z
// 实际上up并非相机的上方 而是永远为(0,1,0)
// 而相机的正上方向是靠计算得出的
// VIEW = Rotate * Translate
Matrix4f View(Vec3f camera, Vec3f center, Vec3f up)
{
	Vec3f z = (camera - center).normalize();//-g
	Vec3f x = cross(up, z).normalize();//g×t
	Vec3f y = cross(z, x).normalize();//t
	Matrix4f R = Matrix4f::identity(4);
	Matrix4f Tr = Matrix4f::identity(4);
	for (int i = 0; i < 3; i++)
	{
		R[0][i] = x[i];
		R[1][i] = y[i];
		R[2][i] = z[i];
		Tr[i][3] = -camera[i];
	}
	return R * Tr;
}

Vec3f homo2vec(col4f m)
{
	Vec3f v;
	v.x = m[0][0];
	v.y = m[1][0];
	v.z = m[2][0];
	return v;
}



