#pragma once
#include "geometry.h"
#include "tgaimage.h"
#include "shader.h"

template <typename Shader> void rasterize(Shader shader, TGAImage& image, float* zbuffer)
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
			int idx = static_cast<int>(P.x + P.y * image.get_width()); //否则在这一行会因为小数部分进位而导致漏绘
			//深度插值
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

void rasterize_depth(ShadowShader shader, TGAImage& image, TGAImage& ssao_frame, float* zbuffer);

void rasterize(ShadowShader shader, TGAImage& image, float* zbuffer);

void rasterize(PhoneShader shader, TGAImage& image, float* zbuffer, float* shadowbuffer);

void rasterize2D(Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color, float alpha, int ybuffer[]);

void rasterize(Vec3f screen_coords[], Vec2f texture_coords[], Vec3f normal_coords[], Vec3f para_light, Matrix4f viewport, TGAImage& image, TGAImage texture_map, float* zbuffer);


