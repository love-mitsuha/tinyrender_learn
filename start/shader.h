#pragma once
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"
class BaseShader
{
public:
	BaseShader(){}
	virtual ~BaseShader(){}
	virtual void get_vertex_info(int face_idx, int vertex_idx, Model model) = 0;
	virtual TGAColor fragment(Vec3f Barycenter) = 0;
private:
	

};


class PhoneShader :public BaseShader
{
private:
	Matrix4f MVP;
	Matrix4f MVP_IT;
	Matrix4f viewport;
	Vec3f screen_coords[3];
	Vec2f texture_coords[3];
	Vec3f normal_coords[3];
	Vec3f light_dir;
	TGAImage texture;
	TGAImage normal_map;
	TGAImage spec_map;
public:
	PhoneShader(Matrix4f _MVP, Matrix4f _viewport, TGAImage _texture, Vec3f _light_dir) {
		MVP = _MVP;
		viewport = _viewport;
		texture = _texture;
		light_dir = _light_dir;
	}
	PhoneShader(Matrix4f _MVP, Matrix4f _viewport, TGAImage _texture, TGAImage _normal_map, Vec3f _light_dir) {
		MVP = _MVP;
		MVP_IT = _MVP.invert_transpose();
		viewport = _viewport;
		texture = _texture;
		light_dir = _light_dir;
		normal_map = _normal_map;
	}
	PhoneShader(Matrix4f _MVP, Matrix4f _viewport, TGAImage _texture, TGAImage _normal_map, TGAImage _spec_map, Vec3f _light_dir) {
		MVP = _MVP;
		MVP_IT = _MVP.invert_transpose();
		viewport = _viewport;
		texture = _texture;
		light_dir = _light_dir;
		spec_map = _spec_map;
		normal_map = _normal_map;
	}
	PhoneShader(){}
	~PhoneShader() override {}

	void get_vertex_info(int face_idx, int vertex_idx, Model model);

	void PhoneShader::get_vertex_info(int vertex_idx, Vec3f obj_coords[], Vec2f _texture_coords[], Vec3f _normal_coords[]);
	
	TGAColor fragment(Vec3f Barycenter);

	TGAColor fragment_load_nm(Vec3f Barycenter);

	Vec3f vertex(int i) { return screen_coords[i]; }
};

class GouraudShader:public BaseShader
{
public:

	GouraudShader(Matrix4f _MVP, Matrix4f _viewport, TGAImage _texture, Vec3f _light_dir)
	{
		MVP = _MVP;
		viewport = _viewport;
		texture = _texture;
		light_dir = _light_dir;
	}
	GouraudShader(){}
	~GouraudShader(){}
	void get_vertex_info(int face_idx, int vertex_idx, Model model);

	void GouraudShader::get_vertex_info(int vertex_idx, Vec3f obj_coords[], Vec2f _texture_coords[], Vec3f _normal_coords[]);


	TGAColor fragment(Vec3f Barycenter);

	Vec3f vertex(int i) { return screen_coords[i]; }

private:
	Matrix4f MVP;
	Matrix4f viewport;
	Vec3f screen_coords[3];
	Vec2f texture_coords[3];
	Vec3f normal_coords[3];
	Vec3f light_dir;
	TGAImage texture;
	TGAImage normal_map;
};
















