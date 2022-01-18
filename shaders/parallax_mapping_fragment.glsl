#version 330 core

#define near 0.1f
#define far 100.0f

layout (location = 0) out vec4 out_frag_color;

in vec2 tex_coord;
in vec3 frag_position_tbn;

in vec3 light_pos_tbn;
in vec3 camera_pos_tbn;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
};

uniform Material material;

uniform float height_scale;
uniform vec3 light_color;

vec2 parallax_mapping(vec2 tex_coords, vec3 view_dir)
{
	const float min_layers = 8;
    const float max_layers = 16;
    float num_layers = mix(max_layers, min_layers, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));  
    
    float layer_depth = 1.0 / num_layers;
    
    float current_layer_depth = 0.0;
    
    vec2 p = view_dir.xy / view_dir.z * height_scale; 
    vec2 delta_tex_coords = p / num_layers;
  
    
    vec2  current_tex_coords     = tex_coords;
    float current_depth_map_value = texture(material.texture_height1, current_tex_coords).r;
      
	int max_iters = 20;
    while(current_layer_depth < current_depth_map_value && max_iters > 0)
    {
        // shift texture coordinates along direction of P
        current_tex_coords -= delta_tex_coords;
        // get depthmap value at current texture coordinates
        current_depth_map_value = texture(material.texture_height1, current_tex_coords).r;  
        // get depth of next layer
        current_layer_depth += layer_depth;  
		max_iters--;
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prev_tex_coords = current_tex_coords + delta_tex_coords;

    // get depth after and before collision for linear interpolation
    float after_depth  = current_depth_map_value - current_layer_depth;
    float before_depth = texture(material.texture_height1, prev_tex_coords).r - current_layer_depth + layer_depth;
 
    // interpolation of texture coordinates
    float weight = after_depth / (after_depth - before_depth);
    vec2 final_tex_coords = prev_tex_coords * weight + current_tex_coords * (1.0 - weight);

    return final_tex_coords;
}

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
	float spec = pow(max(dot(norm, half_way_dir), 0.0f), 16);

	vec3 specular = spec * specular_texture;

	return specular;
}

void main()
{
	float alpha = texture(material.texture_diffuse1, tex_coord).a;
	if (alpha < 0.1f)
	{
		discard;
	}

	vec3 view_dir = normalize(camera_pos_tbn - frag_position_tbn);
	vec3 height_texture = vec3(texture(material.texture_height1, tex_coord));
	
	vec2 parallaxed_tex_coord = parallax_mapping(tex_coord, view_dir);

	vec3 diff_texture = vec3(texture(material.texture_diffuse1, parallaxed_tex_coord));
	vec3 specular_texture = vec3(texture(material.texture_specular1, parallaxed_tex_coord));
	vec3 normal_texture = vec3(texture(material.texture_normal1, parallaxed_tex_coord));
	normal_texture = normal_texture * 2.0f - 1.0f;

	vec3 norm  = normalize(normal_texture);

	vec3 light_dir = normalize(light_pos_tbn - frag_position_tbn);
	vec3 half_way_dir = normalize(light_dir + view_dir);

	float dist = length(light_pos_tbn - frag_position_tbn);
	float attenuation = 1.0f / (0.05f + dist * 0.009f + dist * dist * 0.0032f);
	vec3 result = calc_ambient(diff_texture) + (calc_specular(specular_texture, norm, half_way_dir) + calc_diffuse(diff_texture, norm, light_dir));
	
	out_frag_color = vec4(result * light_color * attenuation, 1.0f);
}