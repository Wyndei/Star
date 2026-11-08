extends Label

@export var player : Player

func _process(_delta: float) -> void:
	text =   "X: " + str(int(floor(player.global_position.x))) + \
			" Y: " + str(int(floor(player.global_position.y))) + \
			" Z: " + str(int(floor(player.global_position.z)))
