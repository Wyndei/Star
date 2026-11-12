extends CharacterBody3D
class_name Cart

@export var target : Node
@export var pivot : Marker3D
@export var follow_dist : Vector2 = Vector2(2.0, 2.5)
@export var follow_speed = 2.0
@export var rotate_speed = 1.0

func _physics_process(delta: float) -> void:
	var dist : float = pivot.global_position.distance_to(target.global_position)
	if dist == 0:
		return
	var dir = (target.global_position - pivot.global_position).normalized()
	if dist > follow_dist.y:
		velocity = dir * follow_speed * dist * delta
	elif dist < follow_dist.x:
		velocity = -dir * follow_speed * dist * delta
	else:
		velocity = Vector3.ZERO
	
	velocity *= Vector3(1,0,1)
	
	global_rotation.y = rotate_toward(global_rotation.y, atan2(-dir.x, -dir.z), rotate_speed * delta)
	
	move_and_collide(velocity)
