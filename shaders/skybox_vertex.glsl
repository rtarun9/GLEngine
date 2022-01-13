#version 330 core

layout (location = 0) in vec3 in_position;

out vec3 tex_coords;

uniform mat4 view_mat;
uniform mat4 projection_mat;

void main()
{
	tex_coords = in_position;
	vec4 result = projection_mat * view_mat * vec4(in_position, 1.0f);
	gl_Position = result.xyww;
}