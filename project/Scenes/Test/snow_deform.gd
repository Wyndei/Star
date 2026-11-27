extends TripleBufferCompute



#func _ready() -> void:
	#await get_tree().process_frame #wait for tree to instantiate subviewport
	##init_shader()
	#RenderingServer.call_on_render_thread(func(): init_shader())
	#
	#RenderingServer.global_shader_parameter_set("FloorDisplace", curr)
#

func _on_floor_deform_sub_v_ready_to_init() -> void:
	await get_tree().process_frame #wait for tree to instantiate subviewport
	init_shader()
	#RenderingServer.call_on_render_thread(func(): init_shader())
	
	RenderingServer.global_shader_parameter_set("FloorDisplace", curr)
