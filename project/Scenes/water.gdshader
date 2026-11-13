shader_type spatial;
render_mode cull_disabled;
//render_mode unshaded;

uniform float depth_intensity : hint_range(0.0, 1.0, 0.01) = 0.1;

uniform vec3 color_shallow : source_color;
uniform vec3 color_deep : source_color;

uniform vec3 color_foam : source_color;
uniform sampler2D foamNoise;
uniform sampler2D foamNoise2;

uniform sampler2D screen_texture : hint_screen_texture;
uniform sampler2D depth_texture : hint_depth_texture;

float fresnel(float amount, vec3 normal, vec3 view)
{
	return pow((1.0 - clamp(dot(normalize(normal), normalize(view)), 0.0, 1.0 )), amount);
}

varying vec3 world_pos;
void vertex() {
	world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
}

void fragment() {
	float depth = texture(depth_texture,SCREEN_UV).r;
	
	depth = PROJECTION_MATRIX[3][2] / (depth + PROJECTION_MATRIX[2][2]);
	depth = VERTEX.z + depth;
	
	float foam_noise_value = texture(foamNoise,world_pos.xz*0.06+TIME*0.01).r * 
	texture(foamNoise2,world_pos.xz*0.06+TIME*-0.02).r;;
	
	float foam_value = (clamp(depth*2.0 - foam_noise_value,-1.0,1.0));
	
	vec3 depth_color = mix(color_shallow,color_deep,clamp(depth*depth_intensity,0.0,1.0));
	vec3 foam_color = mix(color_foam,depth_color,round(foam_value));
	
	vec3 screen_color = texture(screen_texture,SCREEN_UV).rgb;
	
	vec3 fresnel_color = mix(screen_color,foam_color,clamp(fresnel(1.0, NORMAL, VIEW)+0.6,0.0,0.95));
	
	ALBEDO = fresnel_color;
	SPECULAR = 0.0;
}

