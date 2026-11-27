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

float get_ridge(ivec2 prev_p, ivec2 curr_p, ivec2 max_coord);
float get_avg(float red, ivec2 prev_p, ivec2 curr_p, ivec2 max_coord);

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
    new_color.g = get_avg(write_color.r, prev_p, curr_p, max_coord);
    new_color.b = get_ridge(prev_p, curr_p, max_coord);

    // imageStore(Next, p, vec4(new_color, 1.0));


    imageStore(Next, p, vec4(write_color.rgb,1.0));

}

float get_ridge(ivec2 prev_p, ivec2 curr_p, ivec2 max_coord) {

    int step = 1; 

    ivec2 top   = clamp(curr_p + ivec2( 0,  step), ivec2(0), max_coord);
    ivec2 bot   = clamp(curr_p + ivec2( 0, -step), ivec2(0), max_coord);
    ivec2 left  = clamp(curr_p + ivec2(-step,  0), ivec2(0), max_coord);
    ivec2 right = clamp(curr_p + ivec2( step,  0), ivec2(0), max_coord);

    float c = imageLoad(Curr, curr_p).g;
    float t = imageLoad(Curr, top).g;
    float b = imageLoad(Curr, bot).g;
    float l = imageLoad(Curr, left).g;
    float r = imageLoad(Curr, right).g;

    float lap  = (t + b + l + r) - 4.0 * c;

    return lap;
}


float get_avg(float red, ivec2 prev_p, ivec2 curr_p, ivec2 max_coord){
    float c = imageLoad(Curr, curr_p).g;
    float t = imageLoad(Curr, clamp(curr_p + ivec2( 0,  1), ivec2(0), max_coord)).g;
    float b = imageLoad(Curr, clamp(curr_p + ivec2( 0, -1), ivec2(0), max_coord)).g;
    float l = imageLoad(Curr, clamp(curr_p + ivec2(-1,  0), ivec2(0), max_coord)).g;
    float r = imageLoad(Curr, clamp(curr_p + ivec2( 1,  0), ivec2(0), max_coord)).g;

    float avg_neighbor = red + (t + b + l + r) * 0.25;
    
    float slope_softness = 0.05; 
    float diffused_g = mix(c, avg_neighbor, slope_softness);
    return diffused_g;
}
