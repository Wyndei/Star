extends Node3D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	
	var leaf : Leaf = Leaf.new()
	
	leaf.set_pos(Vector2i(2,0))
	
	print(leaf.get_pos())
	


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
