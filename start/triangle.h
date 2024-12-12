#pragma once
#include "Bresenham.h"
#include "geometry.h"

void openTrangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color);

void filledTriangleScan(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color);

Vec3f baryCentric(Vec3f p0, Vec3f p1, Vec3f p2, Vec3f p);

Vec3f baryCentric(Vec2f p0, Vec2f p1, Vec2f p2, Vec2f p);