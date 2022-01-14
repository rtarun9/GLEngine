#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coord;

uniform mat4 model_mat;
uniform mat4 light_space_matrix;

void main()
{
	gl_Position = light_space_matrix * model_mat * vec4(in_pos, 1.0f);
}