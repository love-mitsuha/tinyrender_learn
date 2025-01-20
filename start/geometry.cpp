#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>
#include "geometry.h"
#define M_PI 3.1416


//���ʽת��
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

//�ɹ�һ������ϵת��Ϊ�ӿ�����ϵ
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

// ��������������˶� �����������һ���˶� ���ƽ����ԭ��,����-z
// ʵ����up����������Ϸ� ������ԶΪ(0,1,0)
// ����������Ϸ����ǿ�����ó���
// VIEW = Rotate * Translate
Matrix4f View(Vec3f camera, Vec3f center, Vec3f up)
{
	Vec3f z = (camera - center).normalize();//-g
	Vec3f x = cross(up, z).normalize();//g��t
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

Vec3f rand_point_on_unit_sphere()
{
	float u = (float)rand() / (float)RAND_MAX;
	float v = (float)rand() / (float)RAND_MAX;
	float theta = 2.f * M_PI * u;
	float phi = acos(2.f * v - 1.f);
	return Vec3f(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

float max_elevation_angle(Vec2f p, Vec2f dir, Vec2i W_H, float* zbuffer)
{
	float max_angle = 0;
	for (float t = 0; t < 1000.; t++)
	{
		Vec2f cur = p + dir * t;
		if (cur.x<0 || cur.x>=W_H.x || cur.y<0 || cur.y>=W_H.y)
			continue;
		float distance = (p - cur).norm();
		if (distance < 1.)
			continue;
		float elevation = zbuffer[int(cur.x) + int(cur.y) * W_H.x] - zbuffer[int(p.x) + int(p.y) * W_H.x];
		max_angle = std::max(max_angle, atanf(elevation / distance));
	}
	return max_angle;
}
