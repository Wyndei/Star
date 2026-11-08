extends CharacterBody3D
class_name Player

@export var walk_speed = 5.0
@export var run_speed = 50.0

var speed = 5.0
@export var jump_speed = 5.0

# Get the gravity from the project settings to be synced with RigidBody nodes.
var gravity = ProjectSettings.get_setting("physics/3d/default_gravity")


@onready var camera = $Camera


@export var mouse_sensitivity = 0.5

var mouse_visible : bool = false

var sprinting : bool = false

func _unhandled_input(event):
	if event is InputEventMouseMotion:
		if not mouse_visible:
			
			camera.rotate_x(deg_to_rad(-event.relative.y*mouse_sensitivity))
			rotate_y(deg_to_rad(-event.relative.x*mouse_sensitivity))
			
			camera.rotation.x = clamp(camera.rotation.x,-1.5,1.5)

func _process(_delta):
	if Input.is_action_just_pressed("Quit"):
		get_tree().quit()
	
	if Input.is_action_just_pressed("ShowMouse"):
		mouse_visible = !mouse_visible
	
	if !mouse_visible:
		Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
	else:
		Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	
	

func _physics_process(_delta):
	if Input.is_action_pressed("Jump"):
		velocity.y = jump_speed

	elif Input.is_action_pressed("Crouch"):
		velocity.y = -jump_speed
	else:
		velocity.y = 0.0


	var input_dir = Input.get_vector("Left", "Right", "Forward", "Back")
	var direction = (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
	if direction:
		velocity.x = direction.x * speed
		velocity.z = direction.z * speed
	else:
		velocity.x = move_toward(velocity.x, 0, speed)
		velocity.z = move_toward(velocity.z, 0, speed)
	
	
	
	
	if Input.is_action_pressed("Run"):
		sprinting = true
	
	if !direction and !(Input.is_action_pressed("Run") or Input.is_action_pressed("Jump")):
		sprinting = false
	
	
	if sprinting:
		speed = run_speed
		jump_speed = run_speed
	else:
		speed = walk_speed
		jump_speed = walk_speed
	
	move_and_slide()
