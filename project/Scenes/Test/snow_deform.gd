extends TripleBufferCompute


func _ready() -> void:
	await get_tree().process_frame #wait for tree to instantiate subviewport
	
	init_shader()
	
	RenderingServer.global_shader_parameter_set("FloorDisplace", curr)
