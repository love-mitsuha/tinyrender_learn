#include "tgaimage.h"
#include "Bresenham.h"
#include "model.h"
#include <iostream>
#include "geometry.h"
#include "triangle.h"
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(127, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

int main(int argc, char** argv) {
    int width = 800;
    int height = 800;
    TGAImage image(width, height, TGAImage::RGB);
    //line
	/*line_bre(13, 20, 80, 40, image, white);
	line_bre(20, 13, 40, 80, image, red);
	line_bre(80, 40, 13, 20, image, red);
	image.flip_vertically(); 
	image.write_tga_file("output.tga");*/
    //filledTrangle
    /*Vec2i t0[3] = { Vec2i(100, 100),   Vec2i(50, 200),  Vec2i(200, 300) };
    Vec2i t1[3] = { Vec2i(100, 200),  Vec2i(300, 100),   Vec2i(200, 300) };
    Vec2i t2[3] = { Vec2i(100, 600), Vec2i(300, 800), Vec2i(300, 600) };
    filledTrangleScan(t0[0], t0[1], t0[2], image, red);
    filledTrangleBary(t1[0], t1[1], t1[2], image, green);
    image.flip_vertically();
    image.write_tga_file("triangles.tga");*/

    const char* filename = "african_head.obj";
    Model model = Model(filename);
    Vec3f light_dir = Vec3f(0, 0, -1);
    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        Vec2i screen_coords[3];
        for (int j = 0; j < 3; j++) 
        {
            Vec3f world_coords = model.vert(face[j]);
            screen_coords[j] = Vec2i((world_coords.x + 1) * width / 2, (world_coords.y + 1) * height / 2);
        }
        Vec3f face_normal = ((model.vert(face[2]) - model.vert(face[0])) ^ (model.vert(face[1]) - model.vert(face[0]))).normalize();//µãË³Ê±ÕëÅÅÁÐ
        float alpha = face_normal * light_dir;
        if (alpha > 0)
        {
            filledTrangleScan(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor((int)(alpha * 255), (int)(alpha * 255), (int)(alpha * 255), (int)(alpha * 255)));
        }
    }
    image.flip_vertically();
    image.write_tga_file("african_head.tga");



	return 0;
}

