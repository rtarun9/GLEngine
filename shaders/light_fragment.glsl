#version 460 core

out vec4 out_frag_color;

uniform vec3 light_color;

void main()
{
	out_frag_color = vec4(light_color, 1.0f);
}