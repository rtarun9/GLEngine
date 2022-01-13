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

void main()
{
	float alpha = texture(material_t.texture_diffuse1, tex_coord).a;
	if (alpha < 0.1f)
	{
		discard;
	}

	vec3 diff_texture = vec3(texture(material_t.texture_diffuse1, tex_coord));

	vec3 result = vec3(diff_texture);

	out_frag_color = vec4(result, alpha);
}