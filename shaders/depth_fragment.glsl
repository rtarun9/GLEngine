#version 330 core

out vec4 out_frag_color;


#define near 0.1f
#define far 1000.0f

void main()
{
	// convert value to NDC first
	float depth = gl_FragCoord.z;
	float ndc = depth * 2.0f - 1.0f;
	float linear_depth = (2.0f * near * far) / (far + near - ndc * (far - near));

	out_frag_color = vec4(vec3(linear_depth / far), 1.0f);
}