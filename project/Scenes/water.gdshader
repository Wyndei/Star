shader_type spatial;
//render_mode cull_disabled;
//render_mode unshaded;

group_uniforms depth;
uniform float depth_intensity : hint_range(0.0, 1.0, 0.01) = 0.1; // controls the color based on depth
uniform float turbidity  : hint_range(0.0, 1.0, 0.01) = 0.7; // how "cloudy" the water is
uniform float max_turbidity  : hint_range(0.0, 1.0, 0.01) = 0.95;

group_uniforms color;
uniform vec3 color_shallow : source_color;
uniform vec3 color_deep : source_color;
uniform vec3 color_foam : source_color;

group_uniforms edge;
uniform sampler2D edgeNoise;
uniform sampler2D edgeNoise2;

group_uniforms foam;
uniform sampler2D foamNoise;
uniform sampler2D foamNoise2;
uniform sampler2D foamNoiseDisplace;

group_uniforms wave;
uniform float wave_speed : hint_range(0.0, 10.0, 0.01) = 0.75;

uniform sampler2D screen_texture : hint_screen_texture;
uniform sampler2D depth_texture : hint_depth_texture;

float fresnel(float amount, vec3 normal, vec3 view){
	return pow((1.0 - clamp(dot(normalize(normal), normalize(view)), 0.0, 1.0 )), amount);
}

varying vec3 world_pos;
void vertex(){
	world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;

	VERTEX.y += (cos((TIME+0.9) * wave_speed*2.0))*0.25;

}

void fragment(){
	float depth = texture(depth_texture,SCREEN_UV).r;

	// remaps the depth from the camera to the object's surface
	depth = PROJECTION_MATRIX[3][2] / (depth + PROJECTION_MATRIX[2][2]);
	depth = VERTEX.z + depth;

	// surface foam calculations
	float foam_displace = texture(foamNoiseDisplace,world_pos.xz*0.05+TIME*-0.1).r*0.01;
	float foam_value = clamp(-texture(foamNoise,world_pos.xz*0.05+foam_displace+TIME*0.05).r *
	texture(foamNoise2,world_pos.xz*0.025+foam_displace+TIME*0.1).r,-1.0,1.0);

	// gets the edge value based on the depth, and displaces it with noise.
	float edge_noise_value = texture(edgeNoise,world_pos.xz*0.05+foam_displace+TIME*0.01).r *
	texture(edgeNoise2,world_pos.xz*0.05+foam_displace+TIME*-0.02).r;
	float edge_value = (clamp(depth*2.0 - edge_noise_value,-1.0,1.0));

	// wave calculations
	float wave_value = (abs(clamp(depth-(abs(tan(cos(-TIME*wave_speed))*3.0)),-1.0,1.0)) + clamp((sin(-TIME*wave_speed*2.0))+0.7,0.0,1.0))
	 + edge_noise_value*0.5;


	// mix colors based on their depth.
	vec3 depth_color = mix(color_shallow,color_deep,clamp(depth*depth_intensity,0.0,1.0));
	// foam_color is the depth_color mixed with the foam's color based on the edge_value.
	vec3 foam_color = mix(color_foam,depth_color,clamp(round(clamp(edge_value,0.0,1.0) *
													  (clamp(wave_value,0.0,1.0))) + round(foam_value),0.0,1.0));

	// get screen color
	vec3 screen_color = texture(screen_texture,SCREEN_UV).rgb;
	// mix screen color with water color based on fresnel.
	vec3 fresnel_color = mix(screen_color,foam_color,clamp((fresnel(1.0, NORMAL, VIEW)+1.0) * turbidity,0.0,max_turbidity));

	ALBEDO = vec3(fresnel_color);
	//ALBEDO = vec3(clamp(depth,0.0,1.0)); // uncomment to see the depth value.
	//ALBEDO = vec3(clamp(wave_value,0.0,1.0)); // uncomment to see the wave value.

	//ALPHA = clamp(depth * 2.0,0.0,1.0); // blends the water with intersecting terrain

	SPECULAR = 0.0; // discard specular as I want flatter colors, but don't actually use toon shading.
}
