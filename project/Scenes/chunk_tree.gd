@tool
extends ChunkTree

@onready var chunk_update_timer: Timer = $ChunkUpdate

@export var player : CharacterBody3D = null 
var player_pos_buffer : Vector2 = Vector2.ZERO

@export_tool_button("Update") var update_action = update

func _ready() -> void:
	if Engine.is_editor_hint():
		set_process_mode(Node.PROCESS_MODE_DISABLED)
		return
	set_process_mode(Node.PROCESS_MODE_PAUSABLE)
	
	var world_seed : int = randi()
	noise.seed = world_seed
	mountain_noise.seed = world_seed
	
	var player_pos : Vector2i = Vector2i(int(floor(player.position.x)),int(floor(player.position.z))) 
	new_root(Vector2i(player_pos.x,player_pos.y));
	update()
	
	player.global_position.y = get_height(player_pos) + 1.0
	
	#chunk_update_timer.start()
	
func update():
	var player_pos : Vector2i = Vector2i(int(floor(player.position.x)),int(floor(player.position.z)))
	chunk_updates(player_pos)

func subdivide_chunks():
	#print("subdivide")
	if player_pos_buffer.distance_to(Vector2(player.global_position.x,player.global_position.z)) < 10:
		return
	player_pos_buffer = Vector2(player.global_position.x,player.global_position.z)
	
	update()
	

func _on_chunk_update_timeout() -> void:
	#print("timer")
	subdivide_chunks()
