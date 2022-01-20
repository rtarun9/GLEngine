#version 460 core

in vec2 tex_coords;

uniform sampler2D offscreen_texture_sampler;
uniform sampler2D bloom_texture_sampler;

out vec4 frag_color;
uniform float exposure;
uniform float bloom_intensity;

void main()
{
	
    vec3 bloom_res = texture(bloom_texture_sampler, tex_coords).rgb;


    bloom_res *= bloom_intensity;

	vec3 fragment = texture(offscreen_texture_sampler, tex_coords).xyz + bloom_res;
	vec3 tonned_mapping = vec3(1.0f) - exp(-fragment * exposure);

	frag_color.rgb = tonned_mapping;
	//frag_color.rgb =texture(bloom_texture_sampler, tex_coords).rgb;
	frag_color.a = texture(offscreen_texture_sampler, tex_coords).a;
}