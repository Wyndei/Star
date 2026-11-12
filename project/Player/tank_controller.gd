extends CharacterBody3D
class_name TankController



@onready var collision: CollisionShape3D = $Collision

@onready var camera_pivot: Node3D = $CameraPivot
@onready var camera_arm: SpringArm3D = $CameraPivot/CameraArm
@onready var camera_buffer: Node3D = $CameraPivot/CameraArm/CameraBuffer
@onready var camera: Camera3D = $CameraPivot/CameraArm/CameraBuffer/Camera

@export_category("camera")
@export var camera_rotate_speed : float = 0.1
@export var mouse_sensitivity : float = 0.01
@export var controller_sensitivity : float = 5.0
var mouse_visible : bool = false

@export_category("zoom")

@export var mouse_zoom_step = 2.0
@export var keyboard_zoom_step = 1.0
@export var controller_zoom_step = 1.0

@export var zoom_speed : float = 5.0
@export var zoom_amount : float = 10.0
@export var zoom_bounds : Vector2 = Vector2(2.0,25.0)

@export_category("movement")
@export var walk_speeds : Vector2 = Vector2(-2.0, 5.0)
@export var run_speeds : Vector2 = Vector2(-5.0, 12.0)
@export var acceleration = 0.5
var speed = 0.0
var running : bool = false

@export var max_rotate_speed = 0.025
@export var rotate_acceleration = 0.5
var rotate_speed = 0.0

func _unhandled_input(event: InputEvent) -> void:
	if Input.is_action_just_pressed("Quit"):
		get_tree().quit()
	
	if Input.is_action_just_released("ScrollUp"):
		zoom(-1, mouse_zoom_step)
	if Input.is_action_just_released("ScrollDown"):
		zoom(1, mouse_zoom_step)
	
	if event is not InputEventMouseMotion or mouse_visible:
		return
	
	
	camera_pivot.rotate_y(deg_to_rad(-event.relative.x*mouse_sensitivity))
	camera_arm.rotate_x(deg_to_rad(-event.relative.y*mouse_sensitivity))
	
	camera_arm.rotation.x = clamp(camera_arm.rotation.x,-1.25,-0.25)


func zoom(amount : int = 0, step : float = 1.0):
	zoom_amount += amount * step
	zoom_amount = clamp(zoom_amount,zoom_bounds.x,zoom_bounds.y) 


func _process(delta):
	
	if Input.is_action_just_pressed("ShowMouse"):
		mouse_visible = !mouse_visible
	
	if !mouse_visible:
		Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
	else:
		Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	
	camera_arm.spring_length = lerpf(camera_arm.spring_length, zoom_amount, zoom_speed * delta)
	camera.fov = lerp(camera.fov, lerp(60, 50, zoom_amount/zoom_bounds.y), zoom_speed * delta)
	
	
var dir : Vector3 = Vector3.ZERO
var wish_dir : Vector3 = Vector3.ZERO
func _physics_process(delta: float) -> void:
	# Add the gravity.
	if not is_on_floor():
		velocity += get_gravity() * delta
	
	
	var input_dir : Vector2 = Input.get_vector("Left", "Right", "Forward", "Back")
	
	var max_speeds = walk_speeds
	if running:
		max_speeds = run_speeds
	
	var max_speed = lerp(max_speeds.x,max_speeds.y, (1.0-input_dir.y)/2.0)
	
	if input_dir.y:
		wish_dir = Vector3(0, 0, -abs(input_dir.y))
		speed = lerp(speed, max_speed, acceleration * delta)
	else:
		speed = lerp(speed, 0.0, acceleration * delta * 2.0)
	
	dir = (transform.basis * wish_dir).normalized()
	
	velocity.x = (dir * speed).x
	velocity.z = (dir * speed).z
	
	if input_dir.x:
		rotate_speed = lerp(rotate_speed, -input_dir.x, rotate_acceleration * delta)
	else:
		rotate_speed = lerp(rotate_speed, 0.0, rotate_acceleration * delta * 5.0)
	
	rotate_y(rotate_speed * max_rotate_speed)
	
	
	if Input.is_action_just_pressed("Run") and input_dir.y:
		running = true
	if !input_dir.y:
		running = false
	
	
	move_and_slide()
