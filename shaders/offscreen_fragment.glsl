#version 330 core

in vec2 tex_coords;

uniform sampler2D offscreen_texture_sampler;

out vec4 frag_color;

void main()
{
	frag_color = texture(offscreen_texture_sampler, tex_coords);
}