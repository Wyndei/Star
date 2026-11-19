shader_type spatial;


uniform vec3 grass_color : source_color;
uniform vec3 sand_color : source_color;
uniform vec3 cliff_color : source_color;

varying vec3 pos;
varying vec3 world_pos;
varying float height;
varying vec3 normal;
void vertex() {
	pos = VERTEX;
	height = pos.y;
	normal = NORMAL;
}


void fragment() {
	
	float sand_height = clamp(height,0.0,1.0);
	float cliff_normal = clamp(pow(normal.y,0.5),0.0,1.0);
	
	vec3 floor_color = mix(sand_color, grass_color, sand_height);
	vec3 final_color = mix(cliff_color, floor_color, cliff_normal);
	
	ALBEDO = final_color;
}