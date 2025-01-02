#include "shader.h"
#include "geometry.h"

void PhoneShader::Vertex(Vec3f obj_coords[], Vec2f texture_coords[], Vec3f normal_coords[])
{
	for (size_t i = 0; i < 3; i++)
	{
		vertex.model_coords[i] = obj_coords[i];
		vertex.texture_coords[i] = texture_coords[i];
		vertex.normal_coords[i] = normal_coords[i];
		vertex.ndc_coords[i] = perspective_homo2point(transform.MVP * point2homo(obj_coords[i]));
		vertex.screen_coords[i] = perspective_homo2point(transform.viewport * transform.MVP * point2homo(obj_coords[i]));
		vertex.normal_transformed[i] = transform.MVP_IT.get_minor(3, 3) * normal_coords[i];
	}
	transform.TB = get_TB();
	transform.TBN.set_col(0, transform.TB[0]);
	transform.TBN.set_col(1, transform.TB[1]);
	
}



//法线插值
TGAColor PhoneShader::fragment(Vec3f Barycenter)
{
	Vec3f normal = vertex.normal_coords[0] * Barycenter.x + vertex.normal_coords[1] * Barycenter.y + vertex.normal_coords[2] * Barycenter.z;
	varying_uv = vertex.texture_coords[0] * Barycenter.x + vertex.texture_coords[1] * Barycenter.y + vertex.texture_coords[2] * Barycenter.z;
	float alpha = -(normal * light.light_gl) > 0 ? -(normal * light.light_gl) : 0;
	TGAColor color = texture.texture_map.get(varying_uv);
	return color * alpha;
}
//在世界坐标系计算光照
//全局法线图
TGAColor PhoneShader::fragment_global_nm(Vec3f Barycenter)
{
	//片元对应纹理坐标
	varying_uv = vertex.texture_coords[0] * Barycenter.x + vertex.texture_coords[1] * Barycenter.y + vertex.texture_coords[2] * Barycenter.z;
	//获取法线
	TGAColor normal_color(texture.normal_map.get(varying_uv));
	Vec3f normal = color2vec(normal_color);;
	//漫反射
	float diffuse = -normal * light.light_gl > 0 ? -normal * light.light_gl : 0;//normal向外 light向里
	//镜面反射
	Vec3f half = -(light.view_dir + light.light_gl) / (light.view_dir + light.light_gl).norm();//view向里 半程向量h需要反向与normal对齐
	float spec = std::pow(std::max(half * normal, 0.0f), texture.spec_map.get(varying_uv).val);
	//获取纹理贴图
	TGAColor color = texture.texture_map.get(varying_uv);
	return color * (diffuse + 0.6 * spec) + light.ambient;
}
//TODO
//切线空间法线图
TGAColor PhoneShader::fragment_tangent_nm(Vec3f Barycenter)
{
	transform.TBN.set_col(2, (vertex.normal_coords[0] * Barycenter.x + vertex.normal_coords[1] * Barycenter.y + vertex.normal_coords[2] * Barycenter.z).normalize());
	transform.TBN = transform.model.get_minor(3, 3).invert_transpose() * transform.TBN;//model to global
	transform.TBN_IT = transform.TBN.invert();
	light.light_tangent = transform.TBN_IT * light.light_gl;//global to tangent
	light.view_dir_tangent = transform.TBN_IT * light.view_dir;//global to tangent
	varying_uv = vertex.texture_coords[0] * Barycenter.x + vertex.texture_coords[1] * Barycenter.y + vertex.texture_coords[2] * Barycenter.z;
	TGAColor color = texture.texture_map.get(varying_uv);
	Vec3f normal = color2vec(texture.normal_map.get(varying_uv));
	float diffuse = -normal * light.light_tangent > 0 ? -normal * light.light_tangent : 0;
	return color * diffuse;

}


//TGAColor PhoneShader::fragment_tangent_nm(Vec3f Barycenter)
//{
//	Matrix3f A, AI, B;
//	varying_uv = vertex.texture_coords[0] * Barycenter.x + vertex.texture_coords[1] * Barycenter.y + vertex.texture_coords[2] * Barycenter.z;
//	Vec3f bn = (vertex.normal_transformed[0] * Barycenter.x + vertex.normal_transformed[1] * Barycenter.y + vertex.normal_transformed[2] * Barycenter.z).normalize();
//	
//	A[0] = vertex.ndc_coords[1] - vertex.ndc_coords[0];//p0p1
//	A[1] = vertex.ndc_coords[2] - vertex.ndc_coords[0];//p0p2
//	A[2] = bn;//变换后normal
//	AI = A.invert();
//
//	//将纹理空间坐标uv转至ndc空间
//	Vec3f i = AI * Vec3f(vertex.texture_coords[1].x - vertex.texture_coords[0].x, vertex.texture_coords[2].x - vertex.texture_coords[0].x, 0.f);
//	Vec3f j = AI * Vec3f(vertex.texture_coords[1].y - vertex.texture_coords[0].y, vertex.texture_coords[2].y - vertex.texture_coords[0].y, 0.f);
//	B.set_col(0, i.normalize());
//	B.set_col(1, j.normalize());
//	B.set_col(2, bn);
//
//	//获取法线
//	TGAColor normal_color(texture.normal_map.get(varying_uv));
//	Vec3f n = color2vec(normal_color);
//	//将normal从切线空间转至ndc空间
//	Vec3f normal = (B * n).normalize();
//	float diffuse = -normal * light.light_ndc > 0 ? -normal * light.light_ndc : 0;//normal向外 light向里
//	TGAColor color = texture.texture_map.get(varying_uv);
//	return color * (diffuse);
//}



Matrix23f PhoneShader::get_TB()
{
	Matrix23f ret,dE;
	Matrix2f dUV;
	dE[0] = vertex.model_coords[1] - vertex.model_coords[0];
	dE[1] = vertex.model_coords[2] - vertex.model_coords[0];
	dUV[0] = vertex.texture_coords[1] - vertex.texture_coords[0];
	dUV[1] = vertex.texture_coords[2] - vertex.texture_coords[0];
	ret = dUV.invert() * dE;
	return ret;
}


void GouraudShader::get_vertex_info(int vertex_idx, Vec3f obj_coords[], Vec2f _texture_coords[], Vec3f _normal_coords[])
{
	vertex_screen_coords[vertex_idx] = perspective_homo2point(viewport * MVP * point2homo(obj_coords[vertex_idx]));
	texture_coords[vertex_idx] = _texture_coords[vertex_idx];
	vertex_normal_coords[vertex_idx] = _normal_coords[vertex_idx];
}

TGAColor GouraudShader::fragment(Vec3f Barycenter)
{
	TGAColor vertex_color[3];
	float alpha[3];
	int texture_x, texture_y;
	for (int i = 0; i < 3; i++)
	{
		alpha[i] = -(vertex_normal_coords[i] * light_dir) > 0 ? -(vertex_normal_coords[i] * light_dir) : 0;
		texture_x = static_cast<int>(texture_coords[i].x * this->texture_map.get_width());
		texture_y = static_cast<int>((1. - texture_coords[i].y) * this->texture_map.get_height());
		vertex_color[i] = this->texture_map.get(texture_x, texture_y) * alpha[i];
	}
	TGAColor color = vertex_color[0] * Barycenter.x + vertex_color[1] * Barycenter.y + vertex_color[2] * Barycenter.z;
	return color;
}
