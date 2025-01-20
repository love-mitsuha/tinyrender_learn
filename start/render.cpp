#include "geometry.h"
#include "tgaimage.h"
#include "triangle.h"
#include "render.h"


void rasterize2D(Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color,float alpha, int ybuffer[])
{
	int x1 = p1.x;
	int y1 = p1.y;
	int x2 = p2.x;
	int y2 = p2.y;
	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	for (int x = x1; x <= x2; x++)
	{
		float t = (x - x1) / (float)(x2 - x);
		int y = (t * y2 + y1) / (t + 1);
		if (ybuffer[x]<y)
		{
			ybuffer[x] = y;
			image.set(x, 400, color);
		}
	}
	
}

void rasterize(Vec3f screen_coords[], Vec2f texture_coords[], Vec3f normal_coords[], Vec3f para_light, Matrix4f viewport, TGAImage& image, TGAImage texture_map, float* zbuffer)
{
	//bbox����ʹ��int
	int xmin = std::floor(std::min(screen_coords[0].x, std::min(screen_coords[1].x, screen_coords[2].x)));
	int xmax = std::ceil(std::max(screen_coords[0].x, std::max(screen_coords[1].x, screen_coords[2].x)));
	int ymin = std::floor(std::min(screen_coords[0].y, std::min(screen_coords[1].y, screen_coords[2].y)));
	int ymax = std::ceil(std::max(screen_coords[0].y, std::max(screen_coords[1].y, screen_coords[2].y)));
	Vec3f P;//z����������С�� ����ʹ��Vec3f����
	Vec2f P_texture;
	Vec3f P_normal;
	for (P.x = xmin; P.x <= xmax; P.x++)
	{
		for (P.y = ymin; P.y <= ymax; P.y++)
		{
			Vec3f BaryCentric = baryCentric(screen_coords[0], screen_coords[1], screen_coords[2], P);
			int idx = static_cast<int>(P.x + P.y * image.get_width()); //��������һ�л���ΪС�����ֽ�λ������©��
			//��Ȳ�ֵ �����ֵ ���߲�ֵ
			P.z = screen_coords[0].z * BaryCentric.x + screen_coords[1].z * BaryCentric.y + screen_coords[2].z * BaryCentric.z;
			P_texture = texture_coords[0] * BaryCentric.x + texture_coords[1] * BaryCentric.y + texture_coords[2] * BaryCentric.z;
			P_normal = normal_coords[0] * BaryCentric.x + normal_coords[1] * BaryCentric.y + normal_coords[2] * BaryCentric.z;
			//P_normal = homo2vec((M.transpose()).invert() * vec2homo(P_normal));
			float alpha = -(P_normal * para_light) > 0 ? -(P_normal * para_light) : 0;//��������
			int texture_x = static_cast<int>(P_texture.x * texture_map.get_width());
			int texture_y = static_cast<int>((1. - P_texture.y) * texture_map.get_height());//����������������Ͻǿ�ʼ��Ҫ��ת
			TGAColor color = texture_map.get(texture_x, texture_y) * alpha;
			if (BaryCentric.x < 0 || BaryCentric.y < 0 || BaryCentric.z < 0)
				continue;
			if (zbuffer[idx] < P.z)
			{
				zbuffer[idx] = P.z;
				image.set(P.x, P.y, color);
			}
		}
	}


}

void rasterize_depth(ShadowShader shader, TGAImage& image, TGAImage& ssao_frame, float* zbuffer)
{
	{
		Vec3f screen_coords[3];
		for (int i = 0; i < 3; i++)
		{
			screen_coords[i] = shader.get_vertex(i);
		}
		int xmin = std::floor(std::min(screen_coords[0].x, std::min(screen_coords[1].x, screen_coords[2].x)));
		int xmax = std::ceil(std::max(screen_coords[0].x, std::max(screen_coords[1].x, screen_coords[2].x)));
		int ymin = std::floor(std::min(screen_coords[0].y, std::min(screen_coords[1].y, screen_coords[2].y)));
		int ymax = std::ceil(std::max(screen_coords[0].y, std::max(screen_coords[1].y, screen_coords[2].y)));
		Vec3f P;
		for (P.x = xmin; P.x <= xmax; P.x++)
		{
			for (P.y = ymin; P.y <= ymax; P.y++)
			{
				Vec3f BaryCenter = baryCentric(screen_coords[0], screen_coords[1], screen_coords[2], P);
				int idx = static_cast<int>(P.x + P.y * image.get_width()); //��������һ�л���ΪС�����ֽ�λ������©��
				//��Ȳ�ֵ
				P.z = screen_coords[0].z * BaryCenter.x + screen_coords[1].z * BaryCenter.y + screen_coords[2].z * BaryCenter.z;
				if (BaryCenter.x < 0 || BaryCenter.y < 0 || BaryCenter.z < 0)
					continue;
				if (zbuffer[idx] < P.z)
				{
					zbuffer[idx] = P.z;
					image.set(P.x, P.y, shader.fragment(BaryCenter));
					ssao_frame.set(shader.get_varying_uv().x * ssao_frame.get_width(), shader.get_varying_uv().y * ssao_frame.get_height(), TGAColor(255, 255, 255, 255));
				}
			}
		}
	}
}

void rasterize(ShadowShader shader, TGAImage& image, float* zbuffer)
{
	{
		Vec3f screen_coords[3];
		for (int i = 0; i < 3; i++)
		{
			screen_coords[i] = shader.get_vertex(i);
		}
		int xmin = std::floor(std::min(screen_coords[0].x, std::min(screen_coords[1].x, screen_coords[2].x)));
		int xmax = std::ceil(std::max(screen_coords[0].x, std::max(screen_coords[1].x, screen_coords[2].x)));
		int ymin = std::floor(std::min(screen_coords[0].y, std::min(screen_coords[1].y, screen_coords[2].y)));
		int ymax = std::ceil(std::max(screen_coords[0].y, std::max(screen_coords[1].y, screen_coords[2].y)));
		Vec3f P;
		for (P.x = xmin; P.x <= xmax; P.x++)
		{
			for (P.y = ymin; P.y <= ymax; P.y++)
			{
				Vec3f BaryCenter = baryCentric(screen_coords[0], screen_coords[1], screen_coords[2], P);
				int idx = static_cast<int>(P.x + P.y * image.get_width()); //��������һ�л���ΪС�����ֽ�λ������©��
				//��Ȳ�ֵ
				P.z = screen_coords[0].z * BaryCenter.x + screen_coords[1].z * BaryCenter.y + screen_coords[2].z * BaryCenter.z;
				if (BaryCenter.x < 0 || BaryCenter.y < 0 || BaryCenter.z < 0)
					continue;
				if (zbuffer[idx] < P.z)
				{
					zbuffer[idx] = P.z;
					image.set(P.x, P.y, shader.fragment(BaryCenter));
				}
			}
		}
	}
}

void rasterize(PhoneShader shader, TGAImage& image, float* zbuffer, float* shadowbuffer)
{
	Vec3f screen_coords[3];
	for (int i = 0; i < 3; i++)
	{
		screen_coords[i] = shader.get_vertex(i);
	}
	int xmin = std::floor(std::min(screen_coords[0].x, std::min(screen_coords[1].x, screen_coords[2].x)));
	int xmax = std::ceil(std::max(screen_coords[0].x, std::max(screen_coords[1].x, screen_coords[2].x)));
	int ymin = std::floor(std::min(screen_coords[0].y, std::min(screen_coords[1].y, screen_coords[2].y)));
	int ymax = std::ceil(std::max(screen_coords[0].y, std::max(screen_coords[1].y, screen_coords[2].y)));
	Vec3f P;
	for (P.x = xmin; P.x <= xmax; P.x++)
	{
		for (P.y = ymin; P.y <= ymax; P.y++)
		{
			Vec3f BaryCenter = baryCentric(screen_coords[0], screen_coords[1], screen_coords[2], P);
			int idx = static_cast<int>(P.x + P.y * image.get_width()); //��������һ�л���ΪС�����ֽ�λ������©��
			//��Ȳ�ֵ
			P.z = screen_coords[0].z * BaryCenter.x + screen_coords[1].z * BaryCenter.y + screen_coords[2].z * BaryCenter.z;
			if (BaryCenter.x < 0 || BaryCenter.y < 0 || BaryCenter.z < 0)
				continue;
			if (zbuffer[idx] < P.z)
			{
				zbuffer[idx] = P.z;
				image.set(P.x, P.y, shader.fragment_global_nm(BaryCenter));
			}
		}
	}
}