#include "shader.h"
#include "geometry.h"


void PhoneShader::get_vertex_info(int vertex_idx, Vec3f obj_coords[], Vec2f _texture_coords[], Vec3f _normal_coords[])
{
	ndc_coords[vertex_idx] = perspective_homo2vec(MVP * point2homo(obj_coords[vertex_idx]));
	texture_coords[vertex_idx] = _texture_coords[vertex_idx];
	normal_coords[vertex_idx] = _normal_coords[vertex_idx];
	screen_coords[vertex_idx] = homo2vec(viewport * point2homo(ndc_coords[vertex_idx]));
	varying_nrm[vertex_idx] = homo2vec(MVP_IT * vec2homo(normal_coords[vertex_idx]));
	
}

TGAColor PhoneShader::fragment(Vec3f Barycenter)
{
	Vec3f normal = normal_coords[0] * Barycenter.x + normal_coords[1] * Barycenter.y + normal_coords[2] * Barycenter.z;
	Vec2f texture = texture_coords[0] * Barycenter.x + texture_coords[1] * Barycenter.y + texture_coords[2] * Barycenter.z;
	float alpha = -(normal * light_dir) > 0 ? -(normal * light_dir) : 0;
	int texture_x = static_cast<int>(texture.x * this->texture.get_width());
	int texture_y = static_cast<int>((1. - texture.y) * this->texture.get_height());
	TGAColor color = this->texture.get(texture_x, texture_y);
	return color * alpha;
}
//在世界坐标系计算光照
TGAColor PhoneShader::fragment_load_nm(Vec3f Barycenter)
{
	Vec3f normal;
	Vec2f texture = texture_coords[0] * Barycenter.x + texture_coords[1] * Barycenter.y + texture_coords[2] * Barycenter.z;
	TGAColor ambient(5, 5, 5, 255);
	int texture_x = static_cast<int>(texture.x * this->texture.get_width());
	int texture_y = static_cast<int>((1. - texture.y) * this->texture.get_height());
	uv = Vec2i(texture_x, texture_y);
	//获取法线
	TGAColor normal_color(normal_map.get(uv.x, uv.y));
	normal = color2vec(normal_color); 
	float diffuse = -normal * light_dir > 0 ? -normal * light_dir : 0;//normal向外 light向里
	Vec3f half = -(view + light_dir) / (view + light_dir).norm();//view向里 半程向量h需要反向与normal对齐
	//获取反射系数
	float spec = std::pow(std::max(half * normal, 0.0f), spec_map.get(uv.x, uv.y).val);
	//获取纹理
	TGAColor color = this->texture.get(uv.x, uv.y);
	return color * (diffuse + 0.6 * spec) + ambient;
}
//TODO 在ndc空间计算光照？
TGAColor PhoneShader::fragment_tangent_nm(Vec3f Barycenter)
{
	Matrix3f A,AI,B;
	Vec3f n;
	Vec2f texture = texture_coords[0] * Barycenter.x + texture_coords[1] * Barycenter.y + texture_coords[2] * Barycenter.z;
	Vec3f bn = (varying_nrm[0] * Barycenter.x + varying_nrm[1] * Barycenter.y + varying_nrm[2] * Barycenter.z).normalize();
	Vec3f light = homo2vec(MVP * vec2homo(light_dir));

	TGAColor ambient(5, 5, 5, 255);
	int texture_x = static_cast<int>(texture.x * this->texture.get_width());
	int texture_y = static_cast<int>((1. - texture.y) * this->texture.get_height());
	uv = Vec2i(texture_x, texture_y);
	
	A[0] = ndc_coords[1] - ndc_coords[0];//p0p1
	A[1] = ndc_coords[2] - ndc_coords[0];//p0p2
	A[2] = bn;//变换后normal
	AI = A.invert();

	//将纹理坐标uv转换为切线空间基向量
	Vec3f i = AI * Vec3f(texture_coords[1].x - texture_coords[0].x, texture_coords[2].x - texture_coords[0].x, 0.f);
	Vec3f j = AI * Vec3f(texture_coords[1].y - texture_coords[0].y, texture_coords[2].y - texture_coords[0].y, 0.f);
	B.set_col(0, i.normalize());
	B.set_col(1, j.normalize());
	B.set_col(2, bn);

	//获取法线
	TGAColor normal_color(normal_map.get(uv.x, uv.y));
	n = color2vec(normal_color);
	//将normal从切线空间转至相机空间
	Vec3f normal = (B * n).normalize();
	float diffuse = -normal * light > 0 ? -normal * light : 0;//normal向外 light向里
	Vec3f half = -(view + light) / (view + light).norm();//view向里 半程向量h需要反向与normal对齐
	//获取反射系数
	float spec = std::pow(std::max(half * normal, 0.0f), spec_map.get(uv.x, uv.y).val);
	//获取纹理
	TGAColor color = this->texture.get(uv.x, uv.y); 
	return color * (diffuse + 0.6 * spec) + ambient;
}












void GouraudShader::get_vertex_info(int vertex_idx, Vec3f obj_coords[], Vec2f _texture_coords[], Vec3f _normal_coords[])
{
	screen_coords[vertex_idx] = perspective_homo2vec(viewport * MVP * point2homo(obj_coords[vertex_idx]));
	texture_coords[vertex_idx] = _texture_coords[vertex_idx];
	normal_coords[vertex_idx] = _normal_coords[vertex_idx];
}

TGAColor GouraudShader::fragment(Vec3f Barycenter)
{
	TGAColor vertex_color[3];
	float alpha[3];
	int texture_x, texture_y;
	for (int i = 0; i < 3; i++)
	{
		alpha[i] = -(normal_coords[i] * light_dir) > 0 ? -(normal_coords[i] * light_dir) : 0;
		texture_x = static_cast<int>(texture_coords[i].x * this->texture.get_width());
		texture_y = static_cast<int>((1. - texture_coords[i].y) * this->texture.get_height());
		vertex_color[i] = this->texture.get(texture_x, texture_y) * alpha[i];
	}
	TGAColor color = vertex_color[0] * Barycenter.x + vertex_color[1] * Barycenter.y + vertex_color[2] * Barycenter.z;
	return color;
}
