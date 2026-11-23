shader_type spatial;
render_mode unshaded;

uniform sampler2D tex;

uniform sampler2D depth_texture : hint_depth_texture, repeat_disable, filter_linear_mipmap;


void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	float depth_raw = texture(depth_texture, SCREEN_UV).r;
    vec3 ndc = vec3(SCREEN_UV * 2.0 - 1.0, depth_raw);
	vec4 view = INV_PROJECTION_MATRIX * vec4(ndc, 1.0);
    view.xyz /= view.w;
	
	float background_depth = -view.z;
    float surface_depth = -VERTEX.z;
	
	float distance_behind = background_depth - surface_depth;
	
	float depth_fade = clamp(distance_behind, 0.0, 1.0);
	
	vec3 tex_color = texture(tex,UV).rgb;
	
	ALBEDO = tex_color * (-depth_fade+1.0);
}
