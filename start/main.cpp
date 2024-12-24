#include <chrono>
#include <iostream>
#include "geometry.h"
#include "triangle.h"
#include "render.h"
#include "tgaimage.h"
#include "Bresenham.h"
#include "model.h"
#include "shader.h"
#define image "D:\\tinyrender_learn\\image\\"
#define african_head "D:\\tinyrender_learn\\resource\\african_head\\"
#define boggie "D:\\tinyrender_learn\\resource\\boggie\\"
#define diablo3_pose "D:\\tinyrender_learn\\resource\\diablo3_pose\\"
#define fulilian "D:\\tinyrender_learn\\resource\\fu\\"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(127, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

const int view_x = 200;
const int view_y = 200;
const int view_width = 1200;
const int view_height = 1200;
const int width = 1600;
const int height = 1600;
const int texture_width = 1024;
const int texture_height = 1024;
const int normal_map_width = 1024;
const int normal_map_height = 1024;
const int spec_map_width = 1024;
const int spec_map_height = 1024;
float *zbuffer = NULL;
Vec3f light_dir = Vec3f(-1, -1, -1).normalize();
Vec3f camera = Vec3f(-1, 1, 3);
Vec3f center = Vec3f(0, 0, 0);
Vec3f up = Vec3f(0, 1, 0);
TGAImage canvas(width, height, TGAImage::RGB);

int main(int argc, char** argv) {

    auto start = std::chrono::high_resolution_clock::now();

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

    

    //GouraudShader shader_gouarud(MVP, viewport, texture, light_dir);
    //for (int i = 0; i < model.nfaces(); i++)
    //{
    //    std::vector<int> face = model.face(i);//�涥����������
    //    std::vector<int> face_texture = model.face_texture(i);//��Ķ�����������
    //    std::vector<int> face_normal = model.face_normals(i);//��Ķ��㷨������
    //    Vec3f obj_coords[3];
    //    Vec3f normal_coords[3];
    //    Vec2f texture_coords[3];
    //    for (int j = 0; j < 3; j++)
    //    {
    //        obj_coords[j] = model.vert(face[j]);
    //        texture_coords[j] = model.get_texture(face_texture[j]);
    //        normal_coords[j] = model.get_normals(face_normal[j]);
    //        shader_gouarud.get_vertex_info(j, obj_coords, texture_coords, normal_coords);
    //    }
    //    rasterize(shader_gouarud, canvas, zbuffer);
    //}
    //canvas.flip_vertically();
    //canvas.write_tga_file(image "african_head_gouraud.tga");
    
    Matrix4f modelM = Matrix4f::identity(4);//ģ������ת���������� ģ�ͳ�ʼ�������������غϹ�Ϊ��λ��
    Matrix4f view = View(camera, center, up);//��������ת���������
    Matrix4f projection = Matrix4f::identity(4);//ͶӰ���� û������Զ��ƽ��
    projection[3][2] = -1.f / (camera - center).norm();
    Matrix4f viewport = NDC2view(view_x, view_y, view_width, view_height, (center - camera).norm());
    Matrix4f MVP = projection * view * modelM;
    zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++) {
        zbuffer[i] = std::numeric_limits<float>::lowest();
    }
    


    const char* filename = african_head"african_head.obj";
    Model model_head = Model(filename);
    TGAImage texture_head(texture_width, texture_height, 24);
    texture_head.read_tga_file(african_head"african_head_diffuse.tga");
    TGAImage normal_map_head(normal_map_width, normal_map_height, 32);
    normal_map_head.read_tga_file(african_head"african_head_nm.tga");
    TGAImage spec_map_head(spec_map_width, spec_map_height, 8);
    spec_map_head.read_tga_file(african_head"african_head_spec.tga");

    filename = african_head"african_head_eye_inner.obj";
    Model model_eye_inner = Model(filename);
    TGAImage texture_eye_inner(256, 256, 24);
    texture_eye_inner.read_tga_file(african_head"african_head_eye_inner_diffuse.tga");
    TGAImage normal_map_eye_inner(256, 256, 32);
    normal_map_eye_inner.read_tga_file(african_head"african_head_eye_inner_nm.tga");
    TGAImage spec_map_eye_inner(256, 256, 32);
    spec_map_eye_inner.read_tga_file(african_head"african_head_eye_inner_spec.tga");

    PhoneShader shader_phone(MVP, viewport, texture_head, normal_map_head, spec_map_head, light_dir);

    
    for (int i = 0; i < model_head.nfaces(); i++)
    {
        std::vector<int> face = model_head.face(i);//�涥����������
        std::vector<int> face_texture = model_head.face_texture(i);//��Ķ�����������
        std::vector<int> face_normal = model_head.face_normals(i);//��Ķ��㷨������

        Vec3f obj_coords[3];
        Vec3f normal_coords[3];
        Vec2f texture_coords[3];
        for (int j = 0; j < 3; j++)
        {
            obj_coords[j] = model_head.vert(face[j]);
            texture_coords[j] = model_head.get_texture(face_texture[j]);
            normal_coords[j] = model_head.get_normals(face_normal[j]);
            shader_phone.get_vertex_info(j, obj_coords, texture_coords, normal_coords);
        }
        rasterize(shader_phone, canvas, zbuffer);
    }

    //shader_phone.~PhoneShader();







    canvas.flip_vertically();
    canvas.write_tga_file(image "african_head_phone.tga");




    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "��Ⱦʱ�� " << duration << " seconds" << std::endl;
    return 0;
}

