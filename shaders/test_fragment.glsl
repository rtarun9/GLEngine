#version 330 core

out vec4 out_frag_color;

in vec2 tex_coord;
in vec3 normal;
in vec3 frag_position;

struct material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};

uniform material material_t;

uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 camera_position;


void main()
{

	vec4 diff_texture = texture(material_t.texture_diffuse1, tex_coord);
	vec4 specular_texture = texture(material_t.texture_specular1, tex_coord);

	float alpha = texture(material_t.texture_diffuse1, tex_coord).a;

	const float ambient_strengh = 0.1f;
	vec3 ambient = light_color * ambient_strengh * vec3(diff_texture);

	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(light_position - frag_position);

	float diff_strengh = max(dot(norm, light_direction), 0.0f);
	vec3 diffuse = light_color * diff_strengh * vec3(diff_texture);

	float specular_strength = 0.2f;
	vec3 view_dir = normalize(camera_position - frag_position);
	vec3 reflect_dir = reflect(-light_direction, norm);
	
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), 32.0f);
	vec3 specular = vec3(specular_texture) * spec * light_color * specular_strength;

	float dist = length(abs(frag_position - light_position));

	vec4 result = vec4((ambient + diffuse + specular) * 1.0f / dist, alpha);
	
	out_frag_color = result;
}