#version 460 core

in vec2 tex_coords;

uniform sampler2D offscreen_texture_sampler;
uniform sampler2D bloom_texture_sampler;

out vec4 frag_color;
uniform float exposure;
uniform float bloom_intensity;

void main()
{
    vec2 offset = 1.0f / textureSize(offscreen_texture_sampler, 0);
    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
	
    vec3 bloom_res = texture(bloom_texture_sampler, tex_coords).rgb* weight[0];
	for (int i = 0; i < 5; i++)
	{
		bloom_res += texture(bloom_texture_sampler, tex_coords + vec2(offset.x * i, 0.0f)).rgb * weight[i];
		bloom_res += texture(bloom_texture_sampler, tex_coords - vec2(offset.x * i, 0.0f)).rgb * weight[i];
	}

	for (int i = 0; i < 5; i++)
	{
		bloom_res += texture(bloom_texture_sampler, tex_coords + vec2(0.0f, offset.y * i)).rgb * weight[i];
		bloom_res += texture(bloom_texture_sampler, tex_coords - vec2(0.0f,  offset.y * i)).rgb * weight[i];
	}


    bloom_res *= bloom_intensity;

	vec3 fragment = texture(offscreen_texture_sampler, tex_coords).xyz + bloom_res;
	vec3 tonned_mapping = vec3(1.0f) - exp(-fragment * exposure);

	frag_color.rgb = tonned_mapping;
	frag_color.a = texture(offscreen_texture_sampler, tex_coords).a;
}