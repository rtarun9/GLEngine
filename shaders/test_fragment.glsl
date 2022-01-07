#version 330 core

out vec4 out_frag_color;

uniform float u_time;

in vec2 tex_coord;

uniform sampler2D tex;

void main()
{
	out_frag_color = texture(tex, tex_coord);
}