#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>
#include "geometry.h"

//���ʽת��
Vec4f vec2homo(Vec3f v)
{
	Vec4f m;
	m[0] = v.x;
	m[1] = v.y;
	m[2] = v.z;
	m[3] = 1.f;
	return m;
}

//�ɹ�һ������ϵת��Ϊ�ӿ�����ϵ
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

Vec3f homo2vec(Vec4f m)
{
	Vec3f v;
	v.x = m[0] / m[3];
	v.y = m[1] / m[3];
	v.z = m[2] / m[3];
	return v;
}

