#pragma once
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

struct Vertex
{
	Vec3f model_coords[3];
	Vec2f texture_coords[3];
	Vec3f normal_coords[3];
	Vec3f screen_coords[3];
	Vec3f ndc_coords[3];
	Vec3f normal_transformed[3];
};

struct Transform
{
	Matrix23f TB;
	Matrix3f Rotate;
	Matrix3f TBN;
	Matrix3f TBN_IT;
	Matrix4f viewport;
	Matrix4f model;
	Matrix4f view;
	Matrix4f projection;
	Matrix4f MVP;
	Matrix4f MVP_IT;
	
};

struct Light
{
	Vec3f light_tangent;
	Vec3f view_dir;
	Vec3f view_dir_tangent;
	Vec3f light_gl;
	Vec3f light_ndc;
	TGAColor ambient;
};

struct Texture
{
	TGAImage texture_map;
	TGAImage normal_map;
	TGAImage spec_map;
};


class BaseShader
{
public:
	BaseShader() {}
	virtual ~BaseShader() {}

	virtual TGAColor fragment(Vec3f Barycenter) = 0;
	


private:


};


class PhoneShader : public BaseShader
{
private:
	Vec2f varying_uv;
	Vertex vertex;
	Light light;
	Transform transform;
	Texture texture;
	
public:
	PhoneShader(Light _light, Transform _transform, Texture _texture) 
	{
		light = _light;
		texture = _texture;
		transform = _transform;
		transform.Rotate = transform.MVP.get_minor(3, 3);
		transform.MVP_IT = transform.MVP.invert();
		light.light_ndc = transform.Rotate * light.light_gl;
	}


	PhoneShader(){}
	~PhoneShader() override 
	{
		
	}

	void Vertex(Vec3f obj_coords[], Vec2f texture_coords[], Vec3f normal_coords[]);
	
	Matrix23f get_TB();

	TGAColor fragment(Vec3f Barycenter);

	TGAColor fragment_global_nm(Vec3f Barycenter);

	TGAColor fragment_tangent_nm(Vec3f Barycenter);

	Vec3f get_vertex(int i) { return vertex.screen_coords[i]; }
};




class GouraudShader:public BaseShader
{
public:

	GouraudShader(Matrix4f _MVP, Matrix4f _viewport, TGAImage _texture, Vec3f _light_dir)
	{
		MVP = _MVP;
		viewport = _viewport;
		texture_map = _texture;
		light_dir = _light_dir;
	}
	GouraudShader(){}
	~GouraudShader(){}

	void get_vertex_info(int vertex_idx, Vec3f obj_coords[], Vec2f _texture_coords[], Vec3f _normal_coords[]);

	TGAColor fragment(Vec3f Barycenter);

	Vec3f get_vertex(int i) { return vertex_screen_coords[i]; }

private:
	Matrix4f MVP;
	Matrix4f MVP_IT;
	Matrix4f viewport;
	Vec3f vertex_screen_coords[3];
	Vec2f texture_coords[3];
	Vec3f vertex_normal_coords[3];
	Vec3f light_dir;
	TGAImage texture_map;
	TGAImage normal_map;
	TGAImage spec_map;
};
