#version 330 core

out vec4 out_frag_color;

in vec2 tex_coord;
in vec3 normal;
in vec3 frag_position;

in vec4 light_space_frag_position;

struct material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};

uniform material material_t;

uniform sampler2D depth_map;

uniform vec3 light_color;
uniform vec3 camera_position;

uniform vec3 directional_light_dir;

vec3 calc_ambient(vec3 diff_texture)
{
	const float ambient_strengh = 0.2f;
	vec3 ambient = light_color * ambient_strengh * vec3(diff_texture);
	return ambient;
}

vec3 calc_diffuse(vec3 diff_texture, vec3 light_direction)
{
	vec3 norm = normalize(normal);

	float diff_strengh = max(dot(norm, light_direction), 0.0f);
	vec3 diffuse = light_color * diff_strengh * vec3(diff_texture);
	return diffuse;
}

vec3 calc_specular(vec3 specular_texture, vec3 light_direction)
{
	float specular_strength = 0.1f;
	vec3 view_dir = normalize(camera_position - frag_position);

	vec3 half_way_dir = normalize(light_direction + view_dir);
	vec3 norm = normalize(normal);
	float spec = pow(max(dot(norm, half_way_dir), 0.0f), 64.0f);
	vec3 specular = specular_texture * spec * light_color * specular_strength;

	return specular;
}

float calc_shadow(vec4 light_space_frag_pos)
{
	// do perspective divide as it happens after gl_Position is set but the frag coord is not sent to it.
	vec3 ndc = light_space_frag_pos.xyz / light_space_frag_pos.w;
	// convert from -1 1 to 0 1
	ndc = ndc * 0.5f + 0.5f;
	float closest_depth = texture(depth_map, ndc.xy).x;
	float current_dpeth = ndc.z;

	float shadow = current_dpeth > closest_depth ? 1.0f : 0.0f;
	return shadow;	
}

void main()
{
	float alpha = texture(material_t.texture_diffuse1, tex_coord).a;
	if (alpha < 0.1f)
	{
		discard;
	}
	
	vec3 global_light_dir = normalize(-directional_light_dir);
	
	vec3 diff_texture = vec3(texture(material_t.texture_diffuse1, tex_coord));
	vec3 specular_texture = vec3(texture(material_t.texture_specular1, tex_coord));

	vec3 directional_ambient = calc_ambient(diff_texture);

	vec3 directional_diffuse = calc_diffuse(diff_texture, global_light_dir);

	vec3 directional_specular = calc_specular(specular_texture, global_light_dir);

	vec3 result = vec3(directional_ambient + (1.0f - calc_shadow(light_space_frag_position)) * (directional_diffuse + directional_specular));

	out_frag_color = vec4(result, alpha);
}