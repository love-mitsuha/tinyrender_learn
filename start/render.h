#pragma once
#include "geometry.h"
#include "tgaimage.h"


void rasterize2D(Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color, float alpha, int ybuffer[]);

void rasterize(Vec3f screen_coords[], Vec2f texture_coords[], TGAImage& image, TGAImage texture, float alpha, float* zbuffer);