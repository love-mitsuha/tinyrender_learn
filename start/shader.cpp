#include "shader.h"
#include "geometry.h"

void PhoneShader::Vertex(Vec3f obj_coords[], Vec2f texture_coords[], Vec3f normal_coords[])
{
	for (size_t i = 0; i < 3; i++)
	{
		vertex.model_coords[i] = obj_coords[i];
		vertex.texture_coords[i] = texture_coords[i];
		vertex.normal_coords[i] = normal_coords[i];
		vertex.ndc_coords[i] = homo2point(perspective_div(transform.MVP * point2homo(obj_coords[i])));
		vertex.screen_coords[i] = homo2point(transform.viewport * perspective_div(transform.MVP * point2homo(obj_coords[i])));
		vertex.normal_transformed[i] = transform.MVP_IT.get_minor(3, 3) * normal_coords[i];
	}
	transform.Tangent = get_Tangent();
}

void ShadowShader::Vertex(Vec3f obj_coords[])
{
	for (size_t i = 0; i < 3; i++)
	{
		vertex.model_coords[i] = obj_coords[i];
		vertex.ndc_coords[i] = homo2point(perspective_div(transform.MVP * point2homo(obj_coords[i])));
		vertex.screen_coords[i] = homo2point(transform.viewport * perspective_div(transform.MVP * point2homo(obj_coords[i])));
	}
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
//单通道镜面反射 漫反射 阴影 全局法线
//TGAColor PhoneShader::fragment_global_nm(Vec3f Barycenter)
//{
//	Vec3f cur_screen = vertex.screen_coords[0] * Barycenter.x + vertex.screen_coords[1] * Barycenter.y + vertex.screen_coords[2] * Barycenter.z;
//	Vec3f shadow_screen = homo2point(transform.viewport * perspective_div(transform.gl2shadow * (transform.viewport * transform.MVP).invert() * point2homo(cur_screen)));
//	int idx = int(shadow_screen.x) + int(shadow_screen.y) * transform.width;
//	float shadow = .3 + .7 * (light.shadowbuffer[idx] < shadow_screen.z + 0.1);//z-fighting
//	//片元对应纹理坐标
//	varying_uv = vertex.texture_coords[0] * Barycenter.x + vertex.texture_coords[1] * Barycenter.y + vertex.texture_coords[2] * Barycenter.z;
//	//获取法线
//	TGAColor normal_color(texture.normal_map.get(varying_uv));
//	Vec3f normal = color2normal(normal_color);;
//	//漫反射
//	float diffuse = -normal * light.light_gl > 0 ? -normal * light.light_gl : 0;//normal向外 light向里
//	//镜面反射
//	Vec3f half = -(light.view_dir + light.light_gl) / (light.view_dir + light.light_gl).norm();//view向里 半程向量h需要反向与normal对齐
//	float spec = std::pow(std::max(half * normal, 0.0f), texture.spec_map.get(varying_uv).val);
//	//获取纹理贴图 
//	TGAColor color = texture.texture_map.get(varying_uv);
//	return light.ambient + color * shadow * (diffuse + 0.6 * spec);
//}
//在世界坐标系计算光照
//全局法线图

//三通道镜面反射 漫反射 阴影 发光 全局法线
TGAColor PhoneShader::fragment_global_nm(Vec3f Barycenter)
{
	Vec3f cur_screen = vertex.screen_coords[0] * Barycenter.x + vertex.screen_coords[1] * Barycenter.y + vertex.screen_coords[2] * Barycenter.z;
	Vec3f shadow_screen = homo2point(transform.viewport * perspective_div(transform.gl2shadow * (transform.viewport * transform.MVP).invert() * point2homo(cur_screen)));
	int idx = int(shadow_screen.x) + int(shadow_screen.y) * transform.width;
	float shadow = .3 + .7 * (light.shadowbuffer[idx] < shadow_screen.z + 0.1);//z-fighting
	//片元对应纹理坐标
	varying_uv = vertex.texture_coords[0] * Barycenter.x + vertex.texture_coords[1] * Barycenter.y + vertex.texture_coords[2] * Barycenter.z;
	//获取法线
	TGAColor normal_color(texture.normal_map.get(varying_uv));
	Vec3f normal = color2normal(normal_color);
	//漫反射
	float diffuse = -normal * light.light_gl > 0 ? -normal * light.light_gl : 0;//normal向外 light向里
	//镜面反射
	Vec3f half = -(light.view_dir + light.light_gl) / (light.view_dir + light.light_gl).norm();//view向里 半程向量h需要反向与normal对齐
	float hightlight_coeff = std::pow(std::max(half * normal, 0.0f), 128);// 128是高光指数，用于控制高光的大小和锐度
	TGAColor spec_color = texture.spec_map.get(varying_uv);
	Vec3f spec = Vec3f(spec_color.raw[0] * hightlight_coeff, spec_color.raw[1] * hightlight_coeff, spec_color.raw[2] * hightlight_coeff);
	//发光贴图
	TGAColor glow_color = texture.glow_map.get(varying_uv);
	//获取纹理贴图 
	TGAColor color = texture.texture_map.get(varying_uv);
	int r = std::min(250, std::max(int(color.r * shadow * (diffuse + 0.6 * spec.x) + glow_color.r * 3), 0));
	int g = std::min(250, std::max(int(color.g * shadow * (diffuse + 0.6 * spec.y) + glow_color.g * 3), 0));
	int b = std::min(250, std::max(int(color.b * shadow * (diffuse + 0.6 * spec.z) + glow_color.b * 3), 0));
	return light.ambient + TGAColor(r, g, b, 255);
}
//切线空间中计算法线
TGAColor PhoneShader::fragment_tangent_nm(Vec3f Barycenter)
{
	transform.TBN.set_col(2, (vertex.normal_coords[0] * Barycenter.x + vertex.normal_coords[1] * Barycenter.y + vertex.normal_coords[2] * Barycenter.z).normalize());
	transform.TBN.set_col(0, ((transform.Tangent - transform.TBN.col(2) * (transform.Tangent * transform.TBN.col(2)))).normalize());
	transform.TBN.set_col(1, cross(transform.TBN.col(2), transform.TBN.col(0)));
	transform.TBN = transform.model.get_minor(3, 3).invert_transpose() * transform.TBN;//model to global
	transform.TBN_IT = transform.TBN.invert();

	Vec3f cur_screen = vertex.screen_coords[0] * Barycenter.x + vertex.screen_coords[1] * Barycenter.y + vertex.screen_coords[2] * Barycenter.z;
	Vec3f shadow_screen = homo2point(transform.viewport * perspective_div(transform.gl2shadow * (transform.viewport * transform.MVP).invert() * point2homo(cur_screen)));
	int idx = int(shadow_screen.x) + int(shadow_screen.y) * transform.width;
	float shadow = .3 + .7 * (light.shadowbuffer[idx] < shadow_screen.z + 0.1);//z-fighting
	light.light_tangent = transform.TBN_IT * light.light_gl;//global to tangent
	light.view_dir_tangent = transform.TBN_IT * light.view_dir;//global to tangent
	varying_uv = vertex.texture_coords[0] * Barycenter.x + vertex.texture_coords[1] * Barycenter.y + vertex.texture_coords[2] * Barycenter.z;
	TGAColor color = texture.texture_map.get(varying_uv);
	Vec3f normal = color2normal(texture.normal_map.get(varying_uv));
	float diffuse = -normal * light.light_tangent > 0 ? -normal * light.light_tangent : 0;//漫反射
	Vec3f half = -(light.view_dir + light.light_gl) / (light.view_dir + light.light_gl).norm();
	//normal = transform.TBN * normal;
	//float spec = std::pow(std::max(half * normal, 0.0f), texture.spec_map.get(varying_uv).val);//镜面反射
	return color * shadow * diffuse + light.ambient;
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
//	Vec3f normal = color2normal(normal_color);
//	//将normal从切线空间转至ndc空间
//	Vec3f l = -light.light_ndc;
//	Vec3f n = (B * normal).normalize();
//	Vec3f r = (n * (n * l * 2.f) - l).normalize();
//	float diffuse = n * l > 0 ? n * l  : 0;//normal向外 light向里
//	float spec = std::pow(std::max(r.z, 0.f), texture.spec_map.get(varying_uv).val);
//	TGAColor color = texture.texture_map.get(varying_uv);
//	return color * (diffuse);
//}

TGAColor ShadowShader::fragment(Vec3f Barycenter)
{
	Vec3f p = vertex.screen_coords[0] * Barycenter.x + vertex.screen_coords[1] * Barycenter.y + vertex.screen_coords[2] * Barycenter.z;
	TGAColor white(255, 255, 255, 255);
	TGAColor color = white * 0.6 * p.z;
	return color;
}

Vec3f PhoneShader::get_Tangent()
{
	Vec3f E1, E2;
	float dU1, dU2, dV1, dV2;
	E1 = vertex.model_coords[1] - vertex.model_coords[0];
	E2 = vertex.model_coords[2] - vertex.model_coords[0];
	dU1 = vertex.texture_coords[1].x - vertex.texture_coords[0].x;
	dU2 = vertex.texture_coords[2].x - vertex.texture_coords[0].x;
	dV1 = vertex.texture_coords[1].y - vertex.texture_coords[0].y;
	dV2 = vertex.texture_coords[2].y - vertex.texture_coords[0].y;
	Vec3f T = (E2 * dV1 - E1 * dV2) / (dV1 * dU2 - dV2 * dU1);
	return T;
}


//void GouraudShader::get_vertex_info(int vertex_idx, Vec3f obj_coords[], Vec2f _texture_coords[], Vec3f _normal_coords[])
//{
//	vertex_screen_coords[vertex_idx] = perspective_div(viewport * MVP * point2homo(obj_coords[vertex_idx]));
//	texture_coords[vertex_idx] = _texture_coords[vertex_idx];
//	vertex_normal_coords[vertex_idx] = _normal_coords[vertex_idx];
//}

TGAColor GouraudShader::fragment(Vec3f Barycenter)
{
	TGAColor vertex_color[3];
	float alpha[3];
	int texture_x, texture_y;
	for (int i = 0; i < 3; i++)
	{
		alpha[i] = -(vertex_normal_coords[i] * para_light) > 0 ? -(vertex_normal_coords[i] * para_light) : 0;
		texture_x = static_cast<int>(texture_coords[i].x * this->texture_map.get_width());
		texture_y = static_cast<int>((1. - texture_coords[i].y) * this->texture_map.get_height());
		vertex_color[i] = this->texture_map.get(texture_x, texture_y) * alpha[i];
	}
	TGAColor color = vertex_color[0] * Barycenter.x + vertex_color[1] * Barycenter.y + vertex_color[2] * Barycenter.z;
	return color;
}
