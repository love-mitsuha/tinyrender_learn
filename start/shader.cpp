#include "shader.h"
#include "geometry.h"

void PhoneShader::get_vertex_info(int face_idx, int vertex_idx, Model model)
{
	screen_coords[vertex_idx] = perspective_homo2vec(viewport * MVP * vec2homo(model.get_vertex(face_idx, vertex_idx)));
	texture_coords[vertex_idx] = model.get_texture(face_idx, vertex_idx);
	normal_coords[vertex_idx] = model.get_normal(face_idx, vertex_idx);
}

void PhoneShader::get_vertex_info(int vertex_idx, Vec3f obj_coords[], Vec2f _texture_coords[], Vec3f _normal_coords[])
{
	screen_coords[vertex_idx] = perspective_homo2vec(viewport * MVP * vec2homo(obj_coords[vertex_idx]));
	texture_coords[vertex_idx] = _texture_coords[vertex_idx];
	normal_coords[vertex_idx] = _normal_coords[vertex_idx];
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

TGAColor PhoneShader::fragment_load_nm(Vec3f Barycenter)
{
	Vec3f normal;
	Vec2f texture = texture_coords[0] * Barycenter.x + texture_coords[1] * Barycenter.y + texture_coords[2] * Barycenter.z;
	int texture_x = static_cast<int>(texture.x * this->texture.get_width());
	int texture_y = static_cast<int>((1. - texture.y) * this->texture.get_height());
	TGAColor normal_color(normal_map.get(texture_x, texture_y));
	normal.x = normal_color.r * 2.0 / 255.0 - 1.0;
	normal.y = normal_color.g * 2.0 / 255.0 - 1.0;
	normal.z = normal_color.b * 2.0 / 255.0 - 1.0;
	Vec3f n = perspective_homo2vec(MVP_IT * vec2homo(normal));
	Vec3f l = perspective_homo2vec(MVP * vec2homo(light_dir));
	Vec3f r = (n * (n * l * 2.f) - l).normalize();
	float diffuse = -(normal * light_dir) > 0 ? -(normal * light_dir) : 0;
	float spec = std::pow(std::max(r.z, 0.0f), spec_map.get(texture_x, texture_y).val);
	TGAColor color = this->texture.get(texture_x, texture_y);
	return color * (diffuse + 0.6 * spec);
}



void GouraudShader::get_vertex_info(int face_idx, int vertex_idx, Model model)
{
	screen_coords[vertex_idx] = perspective_homo2vec(viewport * MVP * vec2homo(model.get_vertex(face_idx, vertex_idx)));
	texture_coords[vertex_idx] = model.get_texture(face_idx, vertex_idx);
	normal_coords[vertex_idx] = model.get_normal(face_idx, vertex_idx);
}

void GouraudShader::get_vertex_info(int vertex_idx, Vec3f obj_coords[], Vec2f _texture_coords[], Vec3f _normal_coords[])
{
	screen_coords[vertex_idx] = perspective_homo2vec(viewport * MVP * vec2homo(obj_coords[vertex_idx]));
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