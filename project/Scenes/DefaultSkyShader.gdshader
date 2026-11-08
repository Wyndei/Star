shader_type sky;

uniform vec3 horizonColor : source_color;
uniform vec3 skyColor : source_color;

uniform vec3 sunColor : source_color;

void sky() {

	float sun_dot = dot(EYEDIR,LIGHT0_DIRECTION);

	vec3 sun_bloom = sunColor * pow(clamp(sun_dot,0.0,1.0),20.0);
	vec3 sun = vec3(clamp(ceil(sun_dot-0.9925),0.0,1.0)*0.5 + sun_dot*0.05) + sun_bloom;


	vec3 final = mix(horizonColor,skyColor,pow(abs((SKY_COORDS.y*2.0)-1.0),0.8));
	COLOR = final + sun;
}
