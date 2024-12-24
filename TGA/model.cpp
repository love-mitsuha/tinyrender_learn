#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char* filename) : verts_(), faces_(), texcoords_(){
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        std::string strash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            verts_.push_back(v);
        }
        //读取纹理坐标
        else if (!line.compare(0,4,"vt  "))
        {
            iss >> strash;
            Vec2f vt;
            for (int i = 0; i < 2; i++)
            {
                iss >> vt[i];
            }
            texcoords_.push_back(vt);
        }
        else if (!line.compare(0,4,"vn  "))
        {
            iss >> strash;
            Vec3f vn;
            for (int i = 0; i < 3; i++)
            {
                iss >> vn[i];
            }
            normals_.push_back(vn);
        }
        else if (!line.compare(0, 2, "f ")) 
        {
            std::vector<int> f;
            std::vector<int> ft;//存储纹理坐标索引
            std::vector<int> fn;//存储法线索引
            int itrash, idx, idx_t, idx_n;
            iss >> trash;
            while (iss >> idx >> trash >> idx_t >> trash >> idx_n) {
                idx--; // in wavefront obj all indices start at 1, not zero
                idx_t--;
                idx_n--;
                f.push_back(idx);
                ft.push_back(idx_t);
                fn.push_back(idx_n);
            }
            faces_.push_back(f);
            faces_texcoords.push_back(ft);
            faces_normals.push_back(fn);
        }
    }
    std::cerr << "# v# " << verts_.size() << " vt# " << texcoords_.size() <<" vn# " <<normals_.size()<< " f# " << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::get_texture(int i)
{
    return texcoords_[i];
}

std::vector<int> Model::face_texture(int idx)
{
    return faces_texcoords[idx];
}

Vec3f Model::get_normals(int i)
{
    return normals_[i];
}

std::vector<int> Model::face_normals(int idx)
{
    return faces_normals[idx];
}

Vec3f Model::get_vertex(int idx, int i)
{
    return verts_[faces_[idx][i]];
}

Vec2f Model::get_texture(int idx, int i)
{
    return texcoords_[faces_texcoords[idx][i]];
}

Vec3f Model::get_normal(int idx, int i)
{
    return normals_[faces_normals[idx][i]];
}