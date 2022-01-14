#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coord;
layout (location = 3) in vec3 in_tangents;
layout (location = 4) in vec3 in_bitangents;

out vec2 tex_coord;
out vec3 normal;
out vec3 frag_position;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

void main()
{
	tex_coord = in_tex_coord;
	normal = mat3(inverse(transpose(model_mat))) * in_normal;
	frag_position = vec3(model_mat * vec4(in_pos, 1.0f));
	gl_Position = projection_mat * view_mat * model_mat * vec4(in_pos, 1.0f);
}