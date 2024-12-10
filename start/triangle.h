#pragma once
#include "Bresenham.h"
#include "geometry.h"

void openTrangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color);

void filledTrangleScan(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color);

void filledTrangleBary(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color);
