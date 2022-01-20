#version 330 core

in vec2 tex_coords;

uniform sampler2D offscreen_texture_sampler;

uniform bool horizontal;
uniform float weight[] =  float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
uniform float spread;

out vec4 frag_color;

void main()
{
    vec2 tex_offset = 0.5f / textureSize(offscreen_texture_sampler, 0);
	vec3 res = texture(offscreen_texture_sampler, tex_coords).rgb * weight[0];
	if (horizontal)
	{
		for (int i = 1; i < 5; i++)
		{
			res += texture(offscreen_texture_sampler, tex_coords + vec2(tex_offset.x * spread * i, 0.0f)).rgb * weight[i];
			res += texture(offscreen_texture_sampler, tex_coords - vec2(tex_offset.x * spread * i, 0.0f)).rgb * weight[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; i++)
		{
			res += texture(offscreen_texture_sampler, tex_coords + vec2(0.0f, tex_offset.y * spread * i)).rgb * weight[i];
			res += texture(offscreen_texture_sampler, tex_coords - vec2(0.0f, tex_offset.y * spread * i)).rgb * weight[i];
		}
	}

	frag_color.rgb =res;
	frag_color.a = 1.0f;
}