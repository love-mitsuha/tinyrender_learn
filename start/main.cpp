#include "tgaimage.h"
#include "Bresenham.h"
#include "model.h"
#include <iostream>
#include "geometry.h"
#include "triangle.h"
#include "render.h"
#define image "D:\\tinyrender_learn\\image\\"
#define african_head "D:\\tinyrender_learn\\resource\\african_head\\"
#define boggie "D:\\tinyrender_learn\\resource\\boggie\\"
#define diablo3_pose "D:\\tinyrender_learn\\resource\\diablo3_pose\\"
#define fulilian "D:\\tinyrender_learn\\resource\\fu\\"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(127, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

const int view_x = 100;
const int view_y = 100;
const int view_width = 600;
const int view_height = 600;
const int width = 800;
const int height = 800;
const int texture_width = 1024;
const int texture_height = 1024;
float *zbuffer = NULL;
Vec3f light_dir = Vec3f(0, 0, -1);
Vec3f camera = Vec3f(0, 0, 3);


int main(int argc, char** argv) {

    /*Vec2i t0[3] = { Vec2i(100, 100),   Vec2i(50, 200),  Vec2i(200, 300) };
    Vec2i t1[3] = { Vec2i(100, 200),  Vec2i(300, 100),   Vec2i(200, 300) };
    Vec2i t2[3] = { Vec2i(100, 600), Vec2i(300, 800), Vec2i(300, 600) };
    filledTrangleScan(t0[0], t0[1], t0[2], image, red);
    filledTrangleBary(t1[0], t1[1], t1[2], image, green);
    image.flip_vertically();
    image.write_tga_file("triangles.tga");*/
    //ybuffer

    /*// just dumping the 2d scene (yay we have enough dimensions!)
    TGAImage scene(width, height, TGAImage::RGB);
    // screen line
    line(Vec2i(10, 10), Vec2i(790, 10), scene, white);
    // scene "2d mesh"
    Vec2i p1(20, 34), p2(744, 400), p3(120, 434), p4(444, 400), p5(330, 463), p6(594, 200);
    line(p1, p2, scene, red);
    line(p3, p4, scene, green);
    line(p5, p6, scene, blue);
    scene.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    scene.write_tga_file("scene.tga");
    
    int ybuffer[800];
    for (int i = 0; i < width; i++) {
        ybuffer[i] = std::numeric_limits<int>::min();
    }
    TGAImage render(width, height, TGAImage::RGB);
    rasterize2D(p1, p2, scene, red, ybuffer);
    rasterize2D(p3, p4, scene, green, ybuffer);
    rasterize2D(p5, p6, scene, blue, ybuffer);
    render.flip_vertically();
    render.write_tga_file("scene2D.tga");*/

    
    
    const char* filename = diablo3_pose"diablo3_pose.obj";
    Model model = Model(filename);
    TGAImage canvas(width, height, TGAImage::RGB);
    zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++) {
        zbuffer[i] = std::numeric_limits<float>::lowest();
    }
    TGAImage texture(texture_width,texture_height,24);
    texture.read_tga_file(diablo3_pose"diablo3_pose_diffuse.tga");

    Matrix4f projection = Matrix4f::identity(4);
    projection[3][2] = -1.f / camera.z;
    Matrix4f viewport = NDC2view(view_x, view_y, view_width, view_height, camera.z);
    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        std::vector<int> face_texture = model.face_texture(i);
        Vec3f world_coords[3];
        Vec3f screen_coords[3];
        Vec2f texture_coords[3];
        TGAColor texture_value[3];
        for (int j = 0; j < 3; j++)
        {
            world_coords[j] = model.vert(face[j]);
            screen_coords[j] = homo2vec(viewport * projection * vec2homo(world_coords[j]));
            texture_coords[j] = model.get_texture(face_texture[j]);
        }
        Vec3f face_normal = cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0])).normalize();//µãÄæÊ±ÕëÅÅÁÐ
        float alpha = face_normal * light_dir;
        if (alpha > 0)
        {
            rasterize(screen_coords, texture_coords, canvas, texture, alpha, zbuffer);
        }
    }
    
    canvas.flip_vertically();
    canvas.write_tga_file(image "diablo3_pose_texture_perspective.tga");

    



    return 0;
}

