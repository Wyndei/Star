shader_type spatial;
render_mode specular_disabled;

render_mode ensure_correct_normals;
//render_mode wireframe;


uniform float chunk_uv_scale;
uniform vec2 offset;

uniform float heat_value;
uniform float wet_value;

uniform sampler2D snow_texture;
uniform sampler2D grass_texture;
uniform sampler2D sand_texture;

uniform sampler2D snow_cliff_texture;
uniform sampler2D cliff_texture;
uniform sampler2D dirt_texture;


group_uniforms index_colors;

uniform vec3 heat_color : source_color;
uniform vec3 cold_color : source_color;
uniform vec3 wet_color : source_color;
uniform vec3 dry_color : source_color;



#include "res://Materials/SDFfuncs.gdshaderinc"

varying vec3 pos;
varying vec3 world_pos;
varying vec3 normal;
void vertex() {
	normal = (vec3(NORMAL).xyz);
	world_pos = (MODEL_MATRIX * vec4(VERTEX*0.06, 1.0)).xyz;
	pos = VERTEX;

}

group_uniforms Ambient_Occlusion;

uniform float angle : hint_range(0.0, 5.0, 0.01) = 0.0;
uniform float radius : hint_range(0.0, 1.0, 0.01) = 0.5;
uniform float freq : hint_range(0.0, 100.0, 0.01) = 0.5;

uniform float AO_intensity : hint_range(0.0, 10.0, 0.01) = 1.0;

void fragment() {

	vec2 base_uv = mix(pos.xy,pos.zy,round(normal.x));
	base_uv = mix(base_uv,pos.xz,round(normal.y));


	//vec2 world_pos_offset = ((world_pos.xz+chunk_uv_scale*0.5) - offset) / chunk_uv_scale;
	//vec2 world_pos_offset = ((world_pos.xz + offset + chunk_uv_scale*0.5)) / chunk_uv_scale;

	float heat_value_normalized = (heat_value+1.0) * 0.5;
	float wet_value_normalized = (wet_value + 1.0) * 0.5;


	vec3 snow = texture(snow_texture,world_pos.xz).rgb;
	vec3 grass = texture(grass_texture,world_pos.xz).rgb;
	vec3 dirt = texture(dirt_texture,world_pos.xz).rgb;

	vec3 snow_cliff = texture(snow_cliff_texture,world_pos.xz).rgb;
	vec3 cliff = texture(cliff_texture,world_pos.xz).rgb;
	vec3 sand = texture(sand_texture,world_pos.xz).rgb;


	float beach_value = clamp(((pos.y)*0.75-1.0) ,0.0,1.0);
	float beach_cliff_value = clamp(((pos.y)*0.75+0.5) ,0.0,1.0);

	vec3 snow_grass = mix(grass,snow,clamp((heat_value_normalized-0.5)*2.0,0.0,1.0));
	//vec3 snow_grass_sand = mix(snow_grass,sand,clamp((heat_value_normalized-0.5)*2.0,0.0,1.0));

	vec3 snow_grass_sand = mix(sand,snow_grass,round(clamp((beach_value),0.0,1.0)));

	vec3 cliff_snow_grass = mix(cliff,snow_cliff,clamp((heat_value_normalized-0.5)*2.0,0.0,1.0));
	//vec3 cliff_snow_grass_sand = mix(cliff_snow_grass,dirt,clamp((heat_value_normalized-0.5)*2.0,0.0,1.0));

	vec3 cliff_snow_grass_sand = mix(dirt,cliff_snow_grass,round(clamp((beach_cliff_value),0.0,1.0)));

	vec3 heat_index = mix(cold_color,heat_color,heat_value_normalized);
	vec3 wet_index = mix(wet_color,dry_color,wet_value_normalized);

	vec3 index = mix(heat_index,wet_index,0.5);

	vec3 final = mix(cliff_snow_grass_sand,snow_grass_sand,round(clamp(pow(normal.y*1.15,8.0)-0.1,0.0,1.0)));


	//AO_LIGHT_AFFECT = ((hatch(base_uv, angle, radius, freq))) * AO_intensity;

	ALBEDO = final;

}
