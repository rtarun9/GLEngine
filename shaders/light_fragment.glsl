#version 460 core

layout (location = 0) out vec4 out_frag_color;
layout (location = 1) out vec4 out_bright_color;

uniform vec3 light_color;
uniform float light_intensity;

void main()
{
	out_frag_color = vec4(light_color * light_intensity * 5.0f, 1.0f);

	float brightness = dot(out_frag_color.xyz, vec3(0.2126, 0.7152, 0.0722));

	if (brightness > 1.0f)
	{
		out_bright_color = vec4(out_frag_color.xyz * light_intensity, 1.0f);
	}
	else
	{
		out_bright_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}