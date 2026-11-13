shader_type spatial;
render_mode unshaded;

varying vec3 normal;
void vertex() {
	normal = NORMAL;
}

void fragment() {
	ALBEDO = normal;
}