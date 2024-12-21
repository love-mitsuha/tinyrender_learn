#pragma once
#include "geometry.h"
#include "tgaimage.h"


void rasterize2D(Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color, float alpha, int ybuffer[]);

void rasterize(Vec3f screen_coords[], Vec2f texture_coords[], Vec3f normal_coords[], Vec3f light_dir, Matrix4f viewport, TGAImage& image, TGAImage texture, float* zbuffer);