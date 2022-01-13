#version 330 core

out vec4 frag_color;

in vec3 tex_coords;

uniform samplerCube sky_box;

void main()
{
	frag_color = texture(sky_box, tex_coords);
}