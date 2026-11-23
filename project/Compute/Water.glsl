#[compute]
#version 450



layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;


layout(set = 0, binding = 3, rgba16f) uniform readonly image2D Write;

layout(set = 0, binding = 0, rgba16f) uniform readonly image2D Prev;

layout(set = 0, binding = 1, rgba16f) uniform readonly image2D Curr;

layout(set = 0, binding = 2, rgba16f) uniform writeonly image2D Next;

layout(push_constant) uniform PushConstants {
    float prev_offset_x;
    float prev_offset_y;
    float curr_offset_x;
    float curr_offset_y; 
} pc;


float get_ripple(ivec2 prev_p, ivec2 curr_p, ivec2 max_coord);

void main(){
    // r: new, g: old, b: ridge

    ivec2 p = ivec2(gl_GlobalInvocationID.xy);

    ivec2 size = imageSize(Curr);
    
    if (p.x >= size.x || p.y >= size.y) {
        return;
    }

    ivec2 max_coord = size - ivec2(1);

    ivec2 move_delta = ivec2(pc.curr_offset_x, pc.curr_offset_y) - ivec2(pc.prev_offset_x, pc.prev_offset_y);
    ivec2 curr_p = clamp(p - move_delta, ivec2(0), max_coord);
    ivec2 prev_p = clamp(curr_p - move_delta, ivec2(0), max_coord);

    vec4 curr_color  = imageLoad(Curr, curr_p);
    vec4 write_color = imageLoad(Write, p);

    vec3 new_color = vec3(0.0);
    
    new_color.r = write_color.r;
    // new_color.g = write_color.r + curr_color.g;
    new_color.b = write_color.r + get_ripple(prev_p, curr_p, max_coord);

    imageStore(Next, p, vec4(new_color, 1.0));
}


float get_ripple(ivec2 prev_p, ivec2 curr_p, ivec2 max_coord) {
    int step = 5;

    ivec2 top   = clamp(curr_p + ivec2( 0,  step), ivec2(0), max_coord);
    ivec2 bot   = clamp(curr_p + ivec2( 0, -step), ivec2(0), max_coord);
    ivec2 left  = clamp(curr_p + ivec2(-step,  0), ivec2(0), max_coord);
    ivec2 right = clamp(curr_p + ivec2( step,  0), ivec2(0), max_coord);

    float pc = imageLoad(Prev, prev_p).b;

    float cc = imageLoad(Curr, curr_p).b;
    float ct = imageLoad(Curr, top).b;
    float cb = imageLoad(Curr, bot).b;
    float cl = imageLoad(Curr, left).b;
    float cr = imageLoad(Curr, right).b;

    float damp = 0.999;
    float c2   = 0.001;

    float lap  = (ct + cb + cl + cr) - 4.0 * cc;
    float next = (2.0 * cc - pc + c2 * lap) * damp;

    return next;
}








