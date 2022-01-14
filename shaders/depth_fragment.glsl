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
};

uniform material material_t;

vec3 linear_depth(float frag_z_coord)
{
	// convert depth buffer to a linear one by reversing projection of z value
	float ndc = gl_FragCoord.z * 2.0f - 1.0f;
	float depth = (2.0f * near * far) / (far + near - ndc * (far - near));
	vec3 result = vec3(depth / far);

	return result;
}


void main()
{
	float alpha = texture(material_t.texture_diffuse1, tex_coord).a;
	if (alpha < 0.1f)
	{
		discard;
	}

	vec3 diff_texture = vec3(texture(material_t.texture_diffuse1, tex_coord));

	vec3 result = diff_texture;

	out_frag_color = vec4(result, 1.0f);
}