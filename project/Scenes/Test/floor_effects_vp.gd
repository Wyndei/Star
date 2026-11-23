extends SubViewport

@export var shader_path : String

@export var pivot : Node3D
@export var cam : Camera3D
@export var target : Node3D

var texel_size : Vector2
var curr_texel : Vector2
var prev_texel : Vector2 
var pos_buffer : Vector2 = Vector2.ZERO


func _ready() -> void:
	await get_tree().process_frame #wait for tree to instantiate subviewport
	
	
	texel_size = Vector2(cam.size / size.x, cam.size / size.y)
	RenderingServer.global_shader_parameter_set("FloorCamWidth", cam.size)

func _process(_delta: float) -> void:
	
	curr_texel = Vector2i(floori(target.global_position.x / texel_size.x),floori(target.global_position.z / texel_size.y))
	prev_texel = Vector2i(floori(pos_buffer.x / texel_size.x),floori(pos_buffer.y / texel_size.y))
	
	
	pivot.global_position = Vector3(curr_texel.x * texel_size.x, target.global_position.y, curr_texel.y * texel_size.y)
	pos_buffer = Vector2(target.global_position.x, target.global_position.z)
	
	RenderingServer.global_shader_parameter_set("FloorCamPos", Vector2(curr_texel.x * texel_size.x, curr_texel.y * texel_size.y))
	
	pos_buffer = Vector2(target.global_position.x,target.global_position.z)
