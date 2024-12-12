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
                std::cout << vt[i]<<std::endl;
            }
            texcoords_.push_back(vt);
        }
        else if (!line.compare(0, 2, "f ")) 
        {
            std::vector<int> f;
            std::vector<int> ft;//存储纹理坐标索引
            int itrash, idx, idx_t;
            iss >> trash;
            while (iss >> idx >> trash >> idx_t >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                idx_t--;
                f.push_back(idx);
                ft.push_back(idx_t);
            }
            faces_.push_back(f);
            faces_texcoords.push_back(ft);
        }
    }
    std::cerr << "# v# " << verts_.size() << " vt# " << texcoords_.size() << " f# " << faces_.size() << std::endl;
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
