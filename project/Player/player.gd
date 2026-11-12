extends CharacterBody3D
class_name Player

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

const SPEED = 5.0
const JUMP_VELOCITY = 4.5


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
	
	

func _physics_process(delta: float) -> void:
	# Add the gravity.
	if not is_on_floor():
		velocity += get_gravity() * delta

	# Handle jump.
	if Input.is_action_just_pressed("ui_accept") and is_on_floor():
		velocity.y = JUMP_VELOCITY

	# Get the input direction and handle the movement/deceleration.
	# As good practice, you should replace UI actions with custom gameplay actions.
	var input_dir := Input.get_vector("Left", "Right", "Forward", "Back")
	var direction := (camera_pivot.transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
	if direction:
		velocity.x = direction.x * SPEED
		velocity.z = direction.z * SPEED
	else:
		velocity.x = move_toward(velocity.x, 0, SPEED)
		velocity.z = move_toward(velocity.z, 0, SPEED)

	move_and_slide()
