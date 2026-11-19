shader_type spatial;
//render_mode wireframe;

global uniform sampler2D FloorDisplace : repeat_disable, filter_nearest;
global uniform vec2 FloorCamPos;
global uniform float FloorCamWidth;

global uniform sampler2D FloorHeight : repeat_disable, filter_nearest;


uniform vec3 floor_color : source_color;
uniform vec3 top_color : source_color;

uniform float ridge_height : hint_range(0.0, 1.0, 0.01) = 0.1;

varying float displace_height;
varying vec3 floor_displace;
varying vec3 world_pos;
void vertex() {
	world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;

	vec2 local_cam_pos = (world_pos.xz - FloorCamPos + FloorCamWidth/2.0)/FloorCamWidth;
	float floor_height = texture(FloorHeight,local_cam_pos).r;
	floor_displace = texture(FloorDisplace,local_cam_pos).rgb;
	
	displace_height = (floor_height - (floor_height * floor_displace.g)) + floor_displace.b * ridge_height;
	VERTEX.y += displace_height;
	

}

void fragment() {
	float floor_threshold = 0.01;
	vec3 color = mix(floor_color,top_color,clamp(displace_height+floor_threshold+floor_displace.b*2.0,0.0,1.0));
	ALBEDO = vec3(color);
	//ALBEDO = vec3(floor_displace);
	ALBEDO = vec3(displace_height * 0.5);
}

//void light() {
//	// Called for every pixel for every light affecting the material.
//	// Uncomment to replace the default light processing function with this one.
//}
