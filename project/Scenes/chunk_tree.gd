@tool
extends ChunkTree

@export var player : Player = null 
var player_pos_buffer : Vector2 = Vector2.ZERO

@export_tool_button("Update") var update_action = update

func _ready() -> void:
	var player_pos : Vector2i = Vector2i(int(floor(player.position.x)),int(floor(player.position.z)))
	new_root(Vector2i(player_pos.x,player_pos.y));
	update()

func update():
	var player_pos : Vector2i = Vector2i(int(floor(player.position.x)),int(floor(player.position.z)))
	chunk_updates(player_pos)

func subdivide_chunks():
	
	if player_pos_buffer.distance_to(Vector2(player.global_position.x,player.global_position.z)) < 50:
		return
	player_pos_buffer = Vector2(player.global_position.x,player.global_position.z)
	
	var player_pos : Vector2i = Vector2i(int(floor(player.position.x)),int(floor(player.position.z)))
	chunk_updates(player_pos)
	



func _on_chunk_subdivide_timeout() -> void:
	subdivide_chunks()
