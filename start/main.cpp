#include "tgaimage.h"
#include "Bresenham.h"
#include "model.h"
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

int main(int argc, char** argv) {
	//line_bre(13, 20, 80, 40, image, white);
	//line_bre(20, 13, 40, 80, image, red);
	//line_bre(80, 40, 13, 20, image, red);
	//image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	//image.write_tga_file("output.tga");
    const char* filename = "african_head.obj";
    Model model=Model(filename);
    int width = 1000;
    int height =1000;
    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vert(face[j]);
            Vec3f v1 = model.vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line_bre(x0, y0, x1, y1, image, white);
        }
    }
    image.flip_vertically();
    image.write_tga_file("african_head.tga");
	return 0;
}

