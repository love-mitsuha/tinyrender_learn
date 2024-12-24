#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec2f> texcoords_;
	std::vector<Vec3f> normals_;
	std::vector<std::vector<int> > faces_;
	std::vector<std::vector<int>> faces_texcoords;
	std::vector<std::vector<int>> faces_normals;

public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
	std::vector<int> face_texture(int idx);
	Vec2f get_texture(int i);
	Vec3f Model::get_normals(int i);
	std::vector<int> Model::face_normals(int idx);
	Vec3f get_vertex(int idx, int i);
	Vec2f get_texture(int idx, int i);
	Vec3f get_normal(int idx, int i);

};

#endif //__MODEL_H__
