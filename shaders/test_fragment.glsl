#version 330 core

#define near 0.1f
#define far 100.0f

out vec4 out_frag_color;

in vec2 tex_coord;
in vec3 normal;
in vec3 frag_position;

struct material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
};

uniform material material_t;

uniform vec3 light_pos;
uniform vec3 camera_pos;
uniform vec3 light_color;

vec3 calc_ambient(vec3 diffuse_texture)
{
	float ambient_strength = 0.3f;
	vec3 ambient = ambient_strength * diffuse_texture;

	return ambient;
}

vec3 calc_diffuse(vec3 diffuse_texture, vec3 norm, vec3 light_dir)
{
	float diff = max(dot(light_dir, norm), 0.0f);
	vec3 diffuse = diff * diffuse_texture;

	return diffuse;
}

vec3 calc_specular(vec3 specular_texture, vec3 norm, vec3 half_way_dir)
{
	float spec = pow(max(dot(norm, half_way_dir), 0.0f), 32.0f);

	vec3 specular = spec * specular_texture * 0.5f;

	return specular;
}

void main()
{
	float alpha = texture(material_t.texture_diffuse1, tex_coord).a;
	if (alpha < 0.1f)
	{
		discard;
	}

	vec3 diff_texture = vec3(texture(material_t.texture_diffuse1, tex_coord));
	vec3 specular_texture = vec3(texture(material_t.texture_specular1, tex_coord));
	vec3 normal_texture = vec3(texture(material_t.texture_normal1, tex_coord));

	vec3 norm  = normalize(normal);
	vec3 light_dir = normalize(light_pos - frag_position);
	vec3 view_dir = normalize(camera_pos - frag_position);
	vec3 half_way_dir = normalize(light_dir + view_dir);

	float atten = length(light_pos - frag_position);

	vec3 result = calc_ambient(diff_texture) + (calc_specular(specular_texture, norm, half_way_dir) + calc_diffuse(diff_texture, norm, light_dir)) * 10.0f / atten;
	result = normal_texture;
	out_frag_color = vec4(result * light_color, 1.0f);
}