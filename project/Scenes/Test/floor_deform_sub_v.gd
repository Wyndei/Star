extends SubViewport

@export var pivot : Node3D
@export var cam : Camera3D
@export var target : Node3D

@export var write_test_rect : TextureRect
@export var prev_test_rect : TextureRect
@export var curr_test_rect : TextureRect
@export var next_test_rect : TextureRect


var rd: RenderingDevice

var write_rid : RID
var a_rid : RID
var b_rid : RID
var c_rid : RID

var write_u : RDUniform
var prev_a_u : RDUniform
var prev_b_u : RDUniform
var prev_c_u : RDUniform
var curr_a_u : RDUniform
var curr_b_u : RDUniform
var curr_c_u : RDUniform
var next_a_u : RDUniform
var next_b_u : RDUniform
var next_c_u : RDUniform

#var write : Texture2DRD
var prev : Texture2DRD
var curr : Texture2DRD
var next : Texture2DRD

var shader : RID
var pipeline : RID

var a_set : RID
var b_set : RID
var c_set : RID

var ping_state : int = 0 # 0 1 2

var shader_ready : bool = false
var pos_buffer : Vector2 = Vector2.ZERO

func _ready() -> void:
	await get_tree().process_frame #wait for tree to instantiate subviewport
	
	init_shader()
	
	RenderingServer.global_shader_parameter_set("FloorCamWidth", cam.size)
	RenderingServer.global_shader_parameter_set("FloorDisplace", curr)

func _process(_delta: float) -> void:
	#pivot.global_position = target.global_position
	#pivot.global_position = Vector3(snappedf(target.global_position.x, cam.size/size.x),target.global_position.y, \
									#snappedf(target.global_position.z, cam.size/size.x))
	#RenderingServer.global_shader_parameter_set("FloorDisplace", get_texture())
	
	if !shader_ready:
		return
	
	write_test_rect.texture = get_texture()
	ping_state = wrapi(ping_state + 1, 0, 3)
	RenderingServer.call_on_render_thread(func(): run_shader(ping_state))


func _on_test_timer_timeout() -> void:
	pass
	#write_test_rect.texture = get_texture()
	#ping_state = wrapi(ping_state + 1, 0, 3)
	#RenderingServer.call_on_render_thread(func(): run_shader(ping_state))



func run_shader(curr_state : int = 0):
	var u_set : RID 
	match curr_state:
		0:
			u_set = a_set
			rd.texture_clear(c_rid,Color(0.0,0.0,0.0,1.0),0,1,0,1)
		1:
			u_set = b_set
			rd.texture_clear(a_rid,Color(0.0,0.0,0.0,1.0),0,1,0,1)
		2:
			u_set = c_set
			rd.texture_clear(b_rid,Color(0.0,0.0,0.0,1.0),0,1,0,1)
	
	var texel_size := Vector2(cam.size / size.x, cam.size / size.y)
	var curr_texel := Vector2i(floori(target.global_position.x / texel_size.x),floori(target.global_position.z / texel_size.y))
	var prev_texel := Vector2i(floori(pos_buffer.x / texel_size.x),floori(pos_buffer.y / texel_size.y))
	
	var push_constant : PackedFloat32Array = PackedFloat32Array([
		curr_texel.x,
		curr_texel.y,
		prev_texel.x,
		prev_texel.y
	])
	var push_constant_bytes = push_constant.to_byte_array()
	
	
	
	var compute_list := rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, u_set, 0)
	
	rd.compute_list_set_push_constant(compute_list, push_constant_bytes, push_constant_bytes.size())

	rd.compute_list_dispatch(compute_list, 32,32,1)
	rd.compute_list_end()
	
	#pivot.global_position = target.global_position
	pivot.global_position = Vector3(curr_texel.x * texel_size.x, target.global_position.y, curr_texel.y * texel_size.y)
	pos_buffer = Vector2(target.global_position.x, target.global_position.z)
	
	RenderingServer.global_shader_parameter_set("FloorCamPos", Vector2(curr_texel.x * texel_size.x, curr_texel.y * texel_size.y))
	

func init_shader():
	
	rd = RenderingServer.get_rendering_device() 
	
	var vp_tex_rid : RID = get_texture().get_rid()
	var vp_rd_rid : RID = RenderingServer.texture_get_rd_texture(vp_tex_rid)
	var vp_format: RDTextureFormat = rd.texture_get_format(vp_rd_rid)
	
	
	var out_format := RDTextureFormat.new()
	out_format.width = vp_format.width
	out_format.height = vp_format.height
	out_format.format = vp_format.format 
	out_format.usage_bits = (
		RenderingDevice.TEXTURE_USAGE_STORAGE_BIT |
		RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT |
		RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT |
		RenderingDevice.TEXTURE_USAGE_CAN_COPY_TO_BIT
	)
	
	var in_format := RDTextureFormat.new()
	in_format.width = vp_format.width
	in_format.height = vp_format.height
	in_format.format = vp_format.format 
	in_format.usage_bits = (
		RenderingDevice.TEXTURE_USAGE_STORAGE_BIT | 
		RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT |
		RenderingDevice.TEXTURE_USAGE_CAN_UPDATE_BIT |
		RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT |
		RenderingDevice.TEXTURE_USAGE_CAN_COPY_TO_BIT
	)
	
	var texture_view := RDTextureView.new()
	write_rid = rd.texture_create(in_format, texture_view, [])
	a_rid = rd.texture_create(out_format, texture_view, [])
	b_rid = rd.texture_create(out_format, texture_view, [])
	c_rid = rd.texture_create(out_format, texture_view, [])
	
#region RDUNIFORM
	
	#prev
	prev_a_u = RDUniform.new()
	prev_a_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	prev_a_u.binding = 0 
	prev_a_u.add_id(a_rid)
	
	prev_b_u = RDUniform.new()
	prev_b_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	prev_b_u.binding = 0 
	prev_b_u.add_id(b_rid)
	
	prev_c_u = RDUniform.new()
	prev_c_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	prev_c_u.binding = 0 
	prev_c_u.add_id(c_rid)

	#curr
	curr_a_u = RDUniform.new()
	curr_a_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	curr_a_u.binding = 1
	curr_a_u.add_id(b_rid)
	
	curr_b_u = RDUniform.new()
	curr_b_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	curr_b_u.binding = 1
	curr_b_u.add_id(c_rid)
	
	curr_c_u = RDUniform.new()
	curr_c_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	curr_c_u.binding = 1
	curr_c_u.add_id(a_rid)
	
	#next
	next_a_u = RDUniform.new()
	next_a_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	next_a_u.binding = 2
	next_a_u.add_id(c_rid)
	
	next_b_u = RDUniform.new()
	next_b_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	next_b_u.binding = 2
	next_b_u.add_id(a_rid)
	
	next_c_u = RDUniform.new()
	next_c_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	next_c_u.binding = 2
	next_c_u.add_id(b_rid)
	
	write_u = RDUniform.new()
	write_u.uniform_type = RenderingDevice.UNIFORM_TYPE_IMAGE
	write_u.binding = 3
	write_u.add_id(vp_rd_rid)
	
#endregion
	
	prev = Texture2DRD.new()
	prev.texture_rd_rid = a_rid
	
	curr = Texture2DRD.new()
	curr.texture_rd_rid = b_rid
	
	next = Texture2DRD.new()                    
	next.texture_rd_rid = c_rid
	
	prev_test_rect.texture = prev
	curr_test_rect.texture = curr
	next_test_rect.texture = next
	
	var new_shader := load("res://Compute/Snow.glsl")
	var spirv: RDShaderSPIRV = new_shader.get_spirv()
	shader = rd.shader_create_from_spirv(spirv)
	pipeline = rd.compute_pipeline_create(shader)
	
	a_set = rd.uniform_set_create([write_u, prev_a_u, curr_a_u, next_a_u], shader, 0)
	b_set = rd.uniform_set_create([write_u, prev_b_u, curr_b_u, next_b_u], shader, 0)
	c_set = rd.uniform_set_create([write_u, prev_c_u, curr_c_u, next_c_u], shader, 0)
	
	rd.texture_clear(a_rid,Color(0.0,0.0,0.0,1.0),0,1,0,1)
	rd.texture_clear(b_rid,Color(0.0,0.0,0.0,1.0),0,1,0,1)
	rd.texture_clear(c_rid,Color(0.0,0.0,0.0,1.0),0,1,0,1)
	
	shader_ready = true
