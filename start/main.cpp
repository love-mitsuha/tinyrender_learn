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
#define M_PI 3.1416

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
const int glow_map_width = 1024;
const int glow_map_height = 1024;
const Vec2i W_H(width, height);
float *zbuffer = NULL;
float* shadowbuffer = NULL;
Vec3f point_light = Vec3f(0, 0, -1);
Vec3f para_light = Vec3f(1, 1, 1).normalize();//appoint to model
Vec3f camera = Vec3f(-1, -1, -3);
Vec3f center = Vec3f(0, 0, 0);
Vec3f up = Vec3f(0, 1, 0);
TGAImage canvas(width, height, TGAImage::RGB);
TGAImage depth(width, height, TGAImage::GRAYSCALE);
TGAImage ssao(width, height, TGAImage::RGB);
TGAImage ssao_frame(texture_width, texture_height, TGAImage::GRAYSCALE);
TGAColor ambient(5, 5, 5, 0);

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
    //    std::vector<int> face = model.face(i);//面顶点坐标索引
    //    std::vector<int> face_texture = model.face_texture(i);//面的顶点纹理索引
    //    std::vector<int> face_normal = model.face_normals(i);//面的顶点法线索引
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

    /*filename = african_head"african_head_eye_inner.obj";
    Model model_eye_inner = Model(filename);
    TGAImage texture_eye_inner(256, 256, 24);
    texture_eye_inner.read_tga_file(african_head"african_head_eye_inner_diffuse.tga");
    TGAImage normal_map_eye_inner(256, 256, 32);
    normal_map_eye_inner.read_tga_file(african_head"african_head_eye_inner_nm.tga");
    TGAImage spec_map_eye_inner(256, 256, 32);
    spec_map_eye_inner.read_tga_file(african_head"african_head_eye_inner_spec.tga");*/
    
    /*SSAO后处理
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (shadowbuffer[x + y * width] < -1e5)
                continue;
            float total = 0;
            for (float angle = 0; angle < 2 * M_PI; angle += M_PI / 4)
            {
                total += M_PI / 2 - max_elevation_angle(Vec2f(x, y), Vec2f(cos(angle), sin(angle)), W_H, shadowbuffer);
            }
            total /= (M_PI / 2) * 8;
            total = pow(total, 15.);
            ssao.set(x, y, TGAColor(255 * total, 255 * total, 255 * total, 255 * total));
        }
    }
    ssao.flip_vertically();
    ssao.write_tga_file(image "diablo3_pose_ssao.tga");
    ssao_frame.flip_vertically();
    ssao_frame.write_tga_file(image "diablo3_pose_ssao_frame.tga");*/

    /*const char* filename = african_head"african_head.obj";
    Model model_head = Model(filename);
    TGAImage texture_head(texture_width, texture_height, 24);
    texture_head.read_tga_file(african_head"african_head_diffuse.tga");
    TGAImage normal_map_head(normal_map_width, normal_map_height, 32);
    normal_map_head.read_tga_file(african_head"african_head_nm.tga");
    TGAImage spec_map_head(spec_map_width, spec_map_height, 8);
    spec_map_head.read_tga_file(african_head"african_head_spec.tga");*/

    const char* filename = diablo3_pose"diablo3_pose.obj";
    Model model_head = Model(filename);
    TGAImage texture_head(texture_width, texture_height, 24);
    texture_head.read_tga_file(diablo3_pose"diablo3_pose_diffuse.tga");
    TGAImage normal_map_head(normal_map_width, normal_map_height, 32);
    normal_map_head.read_tga_file(diablo3_pose"diablo3_pose_nm.tga");
    TGAImage spec_map_head(spec_map_width, spec_map_height, 24);
    spec_map_head.read_tga_file(diablo3_pose"diablo3_pose_spec.tga");
    TGAImage glow_map(glow_map_width, glow_map_height, 24);
    glow_map.read_tga_file(diablo3_pose"diablo3_pose_glow.tga");

    
    Transform transform_shadow;
    shadowbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++) {
        shadowbuffer[i] = std::numeric_limits<float>::lowest();
    }
    Matrix4f modelM_shadow = Matrix4f::identity(4);//模型坐标转换世界坐标 模型初始就与世界坐标重合故为单位阵
    Matrix4f view_shadow = View(point_light, center, up);//世界坐标转换相机坐标
    Matrix4f projection_shadow = Matrix4f::identity(4);//投影矩阵 没有设置远近平面
    projection_shadow[3][2] = -1.f / (point_light - center).norm();
    transform_shadow.model = modelM_shadow;
    transform_shadow.view = view_shadow;
    transform_shadow.projection = projection_shadow;
    transform_shadow.viewport = NDC2view(view_x, view_y, view_width, view_height);
    transform_shadow.MVP = projection_shadow * view_shadow * modelM_shadow;
    ShadowShader shader_shadow(transform_shadow);
    for (int i = 0; i < model_head.nfaces(); i++)
    {
        std::vector<int> face = model_head.face(i);//面顶点坐标索引
        std::vector<int> face_texture = model_head.face_texture(i);//面的顶点纹理索引
        Vec3f obj_coords[3];
        Vec2f texture_coords[3];
        for (int j = 0; j < 3; j++)
        {
            obj_coords[j] = model_head.vert(face[j]);
            texture_coords[j] = model_head.get_texture(face_texture[j]);
        }
        shader_shadow.Vertex(obj_coords, texture_coords);
        rasterize(shader_shadow, depth, shadowbuffer);
    }
    depth.flip_vertically();
    depth.write_tga_file(image "diablo3_pose_depth.tga");
    
    Light light;
    Texture texture;
    Transform transform;
    light.light_gl = para_light;
    light.view_dir = center - camera;
    light.ambient = ambient;
    light.shadowbuffer = shadowbuffer;
    texture.texture_map = texture_head;
    texture.normal_map = normal_map_head;
    texture.spec_map = spec_map_head;
    texture.glow_map = glow_map;
    Matrix4f modelM = Matrix4f::identity(4);//模型坐标转换世界坐标 模型初始就与世界坐标重合故为单位阵
    Matrix4f view = View(camera, center, up);//世界坐标转换相机坐标
    Matrix4f projection = Matrix4f::identity(4);//投影矩阵 没有设置远近平面
    projection[3][2] = -1.f / (camera - center).norm();
    transform.model = modelM;
    transform.view = view;
    transform.projection = projection;
    transform.viewport = NDC2view(view_x, view_y, view_width, view_height);
    transform.MVP = projection * view * modelM;
    transform.gl2shadow = transform_shadow.MVP;
    transform.width = width;
    transform.height = height;
    zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++) {
        zbuffer[i] = std::numeric_limits<float>::lowest();
    }
    PhoneShader shader_phone(light, transform, texture);      
    for (int i = 0; i < model_head.nfaces(); i++)
    {
        std::vector<int> face = model_head.face(i);//面顶点坐标索引
        std::vector<int> face_texture = model_head.face_texture(i);//面的顶点纹理索引
        std::vector<int> face_normal = model_head.face_normals(i);//面的顶点法线索引

        Vec3f obj_coords[3];
        Vec3f normal_coords[3];
        Vec2f texture_coords[3];
        for (int j = 0; j < 3; j++)
        {
            obj_coords[j] = model_head.vert(face[j]);
            texture_coords[j] = model_head.get_texture(face_texture[j]);
            normal_coords[j] = model_head.get_normals(face_normal[j]);
        }
        shader_phone.Vertex(obj_coords, texture_coords, normal_coords);
        rasterize(shader_phone, canvas, zbuffer, shadowbuffer);
    }
    canvas.flip_vertically();
    canvas.write_tga_file(image "diablo3_pose_phone.tga");

    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "渲染时间 " << duration << " seconds" << std::endl;
    return 0;
}

