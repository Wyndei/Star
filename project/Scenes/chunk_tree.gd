extends ChunkTree

@export var player : Player = null 
var player_pos_buffer : Vector2 = Vector2.ZERO


func subdivide_chunks():
	
	#if player_pos_buffer.distance_to(Vector2(player.global_position.x,player.global_position.z)) < 50:
		#return
	player_pos_buffer = Vector2(player.global_position.x,player.global_position.z)
	
	var player_pos : Vector2i = Vector2i(int(floor(player.position.x)),int(floor(player.position.z)))
	chunk_updates(player_pos)
	



func _on_chunk_subdivide_timeout() -> void:
	subdivide_chunks()
