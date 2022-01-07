#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;

out vec2 tex_coord;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

void main()
{
	tex_coord = in_tex_coord;
	gl_Position = projection_mat * view_mat * model_mat * vec4(in_pos, 1.0f);
}