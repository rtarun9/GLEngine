#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coord;
layout (location = 3) in vec3 in_tangents;
layout (location = 4) in vec3 in_bitangents;

out vec2 tex_coord;
out vec3 frag_position_tbn;
out vec3 light_pos_tbn;
out vec3 camera_pos_tbn;

uniform vec3 light_pos;
uniform vec3 camera_pos;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

void main()
{	
	vec3 t = normalize(mat3(model_mat) * in_tangents);
	vec3 b = normalize(mat3(model_mat) * in_bitangents);
	vec3 n = normalize(mat3(model_mat) * in_normal);

	mat3 tbn_mat = transpose(mat3(t, b, n));

	tex_coord = in_tex_coord;
	frag_position_tbn = tbn_mat * vec3(model_mat * vec4(in_pos, 1.0f));
	light_pos_tbn = tbn_mat * light_pos;
	camera_pos_tbn = tbn_mat * camera_pos;

	gl_Position = projection_mat * view_mat * model_mat * vec4(in_pos, 1.0f);
}