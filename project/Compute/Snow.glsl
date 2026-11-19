#[compute]
#version 450



layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;


layout(set = 0, binding = 3, rgba8) uniform readonly image2D Write;

layout(set = 0, binding = 0, rgba8) uniform readonly image2D Prev;

layout(set = 0, binding = 1, rgba8) uniform readonly image2D Curr;

layout(set = 0, binding = 2, rgba8) uniform writeonly image2D Next;

// layout(push_constant, std430) uniform PushConstants {
//     vec2 prev_offset;
//     vec2 curr_offset;
// } pc;

void main(){
    // r: new input, g: old input, b: fx

    vec2 pixel_coord = gl_GlobalInvocationID.xy + vec2(1,1);
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);


    vec4 write_color = imageLoad(Write,p);
    vec4 prev_color = imageLoad(Prev,p);
    vec4 curr_color = imageLoad(Curr,p);
    
    vec3 new_color = vec3(0.0,0.0,0.0);

    if (write_color.rgb != vec3(0,0,0)){
        new_color = curr_color.rgb + vec3(0.0,write_color.r,0.0); // old + new
        imageStore(Next, p, vec4(new_color, 1.0));
    }    

}




