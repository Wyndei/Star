#include "chunktree.h"
#include <godot_cpp/core/class_db.hpp>


using namespace godot;


void ChunkLeaf::_bind_methods() {   
    ClassDB::bind_method(D_METHOD("set_pos", "new_pos"), &ChunkLeaf::set_pos);
    ClassDB::bind_method(D_METHOD("get_pos"), &ChunkLeaf::get_pos);
    ClassDB::bind_method(D_METHOD("get_size"), &ChunkLeaf::get_size);
    ClassDB::bind_method(D_METHOD("set_level, new_level"), &ChunkLeaf::get_level);
    ClassDB::bind_method(D_METHOD("get_level"), &ChunkLeaf::get_level);

    ClassDB::bind_method(D_METHOD("set_tree, new_tree"), &ChunkLeaf::set_tree);
    ClassDB::bind_method(D_METHOD("get_tree"), &ChunkLeaf::get_tree);


    ClassDB::bind_method(D_METHOD("chunk_update", "player_pos"), &ChunkLeaf::chunk_update);
    
    ClassDB::bind_method(D_METHOD("generate_mesh"), &ChunkLeaf::generate_mesh);
    ClassDB::bind_method(D_METHOD("generate_collision"), &ChunkLeaf::generate_collision);
    
    ClassDB::bind_method(D_METHOD("set_mesh", "surface_array"), &ChunkLeaf::set_mesh);
    ClassDB::bind_method(D_METHOD("set_coll", "new_coll"), &ChunkLeaf::set_coll);

    ClassDB::bind_method(D_METHOD("add_to_queue", "subd"), &ChunkLeaf::add_to_queue);
    ClassDB::bind_method(D_METHOD("subdivide"), &ChunkLeaf::subdivide);
    ClassDB::bind_method(D_METHOD("collapse"), &ChunkLeaf::collapse);
    

    ClassDB::bind_method(D_METHOD("should_subdivide", "player_pos"), &ChunkLeaf::should_subdivide);
    ClassDB::bind_method(D_METHOD("should_collapse", "player_pos"), &ChunkLeaf::should_collapse);


    ClassDB::bind_method(D_METHOD("test_visual"), &ChunkLeaf::test_visual);
}

ChunkLeaf::ChunkLeaf() {
}

ChunkLeaf::~ChunkLeaf() {

    if (has_children){
        top_left->queue_free(); 
        top_right->queue_free();
        bot_left->queue_free();
        bot_right->queue_free();
    }

    top_left = nullptr; 
    top_right = nullptr;
    bot_left = nullptr;
    bot_right = nullptr;
    
}

void ChunkLeaf::_ready(){
    // UtilityFunctions::print(pos);
    test_visual();
}

void ChunkLeaf::test_visual(){
    WorkerThreadPool* pool = WorkerThreadPool::get_singleton();
    
    int hlf_size = get_size()/2;

    
    
    // chunk_thread->start(gen_mesh);

    chunk_mesh = memnew(MeshInstance3D); // mesh init

    generate_mesh();

    // Callable gen_mesh = callable_mp(this, &ChunkLeaf::generate_mesh);
    // int64_t task_id = pool->add_task(gen_mesh, false, "mesh_generation");
    // pool->wait_for_task_completion(task_id);

    chunk_coll = memnew(CollisionShape3D); // collision init
    add_child(chunk_mesh);
    
    chunk_mesh->set_position(Vector3(pos.x-hlf_size, 0.0, pos.y-hlf_size));
    chunk_mesh->set_material_override(tree->get_material());


    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    if (level == 0){
        Callable gen_coll = Callable(this, "generate_collision");

        chunk_body = memnew(StaticBody3D); // body init
        add_child(chunk_body);

        chunk_body->add_child(chunk_coll);
        chunk_coll->set_position(Vector3(pos.x-hlf_size, 0.0, pos.y-hlf_size));
        // chunk_coll->set_shape(generate_collision());

    }
    // chunk tester
    // test_mesh = memnew(MeshInstance3D); // marker for each chunk
    // Ref<BoxMesh> new_box_mesh;
    // new_box_mesh.instantiate();
    // add_child(test_mesh);
    // test_mesh->set_mesh(new_box_mesh);
    // test_mesh->set_position(Vector3(pos.x,0.0,pos.y));
}

void ChunkLeaf::chunk_update(Vector2i player_pos){
    // UtilityFunctions::print(player_pos);

    // collapse check
    if (has_children && should_collapse(player_pos)){ // distance check
        add_to_queue(0, player_pos); 
        return;
    } 
    // subdivision checks
    if (level <= 0) return;

    if (!has_children && should_subdivide(player_pos)) add_to_queue(1, player_pos);

    if (!has_children) return;
    update_children(player_pos);
}

void ChunkLeaf::update_children(Vector2i player_pos){
    top_left->chunk_update(player_pos);
    top_right->chunk_update(player_pos);
    bot_left->chunk_update(player_pos);
    bot_right->chunk_update(player_pos);
}

void ChunkLeaf::add_to_queue(bool subd, Vector2i player_pos){
    if (queued) return;

    tree->add_to_queue(this, subd, player_pos);
    queued = true;
}

void ChunkLeaf::subdivide(){
    // UtilityFunctions::print("subd");
    int chunk_size = get_size();
    int new_level = level-1;
    // divide leaf into four children

    int qtr_size = chunk_size/4;

    // instantiate children

    top_left  = memnew(ChunkLeaf);
    top_right = memnew(ChunkLeaf);
    bot_left  = memnew(ChunkLeaf);
    bot_right = memnew(ChunkLeaf);
    
    top_left->set_pos(pos + Vector2i(-qtr_size, qtr_size));
    top_right->set_pos(pos + Vector2i(qtr_size, qtr_size));
    bot_left->set_pos(pos + Vector2i(-qtr_size, -qtr_size));
    bot_right->set_pos(pos + Vector2i(qtr_size, -qtr_size));

    top_left->set_level(new_level);
    top_right->set_level(new_level);
    bot_left->set_level(new_level);
    bot_right->set_level(new_level);

    top_left->set_tree(tree);
    top_right->set_tree(tree);
    bot_left->set_tree(tree);
    bot_right->set_tree(tree);
    
    add_child(top_left);
    add_child(top_right);
    add_child(bot_left);
    add_child(bot_right);

    // hide higher level
    // test_mesh->hide();
    chunk_mesh->hide();
    has_children = true;
    queued = false;
}

void ChunkLeaf::collapse(){
    // UtilityFunctions::print("coll");
    // unhide lower subdiv terrain
    chunk_mesh->show();
    // test_mesh->show();

    // delete all children
    top_left->queue_free(); 
    top_right->queue_free();
    bot_left->queue_free();
    bot_right->queue_free();

    top_left = nullptr; 
    top_right = nullptr;
    bot_left = nullptr;
    bot_right = nullptr;
    has_children = false;
    queued = false;

}

void ChunkLeaf::generate_mesh(){
    // UtilityFunctions::print("mesh_chunk");

    PackedVector3Array vertices;
    PackedVector3Array normals;
    PackedVector2Array uvs;
    PackedInt32Array indices;

    int min_chunk_size = tree->get_min_chunk_size();

    int res = 1 << level;   
    int grid_size = 16;
    float step = (min_chunk_size/grid_size) * res;
    
    int i = 0;
    for(int x = 0; x < grid_size; x++){
        for(int y = 0; y < grid_size; y++){
            
            Vector2 local_coords = pos + Vector2(x,y) * step;

            vertices.push_back(Vector3((x + 0) * step, tree->get_height(local_coords + (Vector2(0,0) * step)), (y + 0) * step)); // create square??
            vertices.push_back(Vector3((x + 1) * step, tree->get_height(local_coords + (Vector2(1,0) * step)), (y + 0) * step));
            vertices.push_back(Vector3((x + 1) * step, tree->get_height(local_coords + (Vector2(1,1) * step)), (y + 1) * step));
            vertices.push_back(Vector3((x + 0) * step, tree->get_height(local_coords + (Vector2(0,1) * step)), (y + 1) * step));
            
            normals.push_back(Vector3(0, 1, 0));
            normals.push_back(Vector3(0, 1, 0));
            normals.push_back(Vector3(0, 1, 0));
            normals.push_back(Vector3(0, 1, 0));

            uvs.push_back(Vector2(0, 1));
            uvs.push_back(Vector2(1, 1));
            uvs.push_back(Vector2(1, 0));
            uvs.push_back(Vector2(0, 0));

            indices.push_back(i+0);
            indices.push_back(i+1);
            indices.push_back(i+2);
            indices.push_back(i+0);
            indices.push_back(i+2);
            indices.push_back(i+3);

            i += 4;
        }
    }
    
    
    Array surface_array;
    surface_array.resize(Mesh::ARRAY_MAX);
    surface_array[Mesh::ARRAY_VERTEX] = vertices;
    surface_array[Mesh::ARRAY_NORMAL] = normals;
    surface_array[Mesh::ARRAY_TEX_UV] = uvs;
    surface_array[Mesh::ARRAY_INDEX] = indices;
    

    call_deferred("set_mesh", surface_array);
    // return arr_mesh;
}

void ChunkLeaf::set_mesh(Array surface_array){
    Ref<ArrayMesh> arr_mesh;
    arr_mesh.instantiate();
    arr_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);

    chunk_mesh->set_mesh(arr_mesh);

    generate_collision();
}

void ChunkLeaf::generate_collision(){
    Ref<Mesh> mesh = chunk_mesh->get_mesh();

    chunk_coll->set_shape(mesh->create_trimesh_shape());
}

void ChunkLeaf::set_coll(Ref<ConcavePolygonShape3D> new_coll){
    chunk_coll->set_shape(new_coll);
}

int ChunkLeaf::get_size(){return tree->get_min_chunk_size() * pow(2,level);}

bool ChunkLeaf::should_subdivide(Vector2i player_pos){if (player_pos.distance_to(pos) < get_size() * tree->get_subd_dist()) return true; else {return false;};}
bool ChunkLeaf::should_collapse(Vector2i player_pos){if (player_pos.distance_to(pos) > get_size() * tree->get_coll_dist()) return true; else {return false;};}


void ChunkTree::_bind_methods() {
    ClassDB::bind_method(D_METHOD("new_root", "root_pos"), &ChunkTree::new_root);
    ClassDB::bind_method(D_METHOD("free_root"), &ChunkTree::free_root);

    ClassDB::bind_method(D_METHOD("set_max_level", "new_max_level"), &ChunkTree::set_max_level);
    ClassDB::bind_method(D_METHOD("get_max_level"), &ChunkTree::get_max_level);

    ClassDB::bind_method(D_METHOD("set_min_chunk_size", "new_min_chunk_size"), &ChunkTree::set_min_chunk_size);
    ClassDB::bind_method(D_METHOD("get_min_chunk_size"), &ChunkTree::get_min_chunk_size);

    ClassDB::bind_method(D_METHOD("set_subd_dist", "new_subd_dist"), &ChunkTree::set_subd_dist); // subdivide distance
    ClassDB::bind_method(D_METHOD("get_subd_dist"), &ChunkTree::get_subd_dist);
    ClassDB::bind_method(D_METHOD("set_coll_dist", "new_coll_dist"), &ChunkTree::set_coll_dist); // collapse distance
    ClassDB::bind_method(D_METHOD("get_coll_dist"), &ChunkTree::get_coll_dist);

    ClassDB::bind_method(D_METHOD("set_material", "new_material"), &ChunkTree::set_material);
    ClassDB::bind_method(D_METHOD("get_material"), &ChunkTree::get_material);

    ClassDB::bind_method(D_METHOD("set_noise", "new_noise"), &ChunkTree::set_noise);
    ClassDB::bind_method(D_METHOD("get_noise"), &ChunkTree::get_noise);

    ClassDB::bind_method(D_METHOD("chunk_updates","player_pos"), &ChunkTree::chunk_updates);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_level"), "set_max_level", "get_max_level");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "min_chunk_size"), "set_min_chunk_size", "get_min_chunk_size");
    
    ADD_PROPERTY(PropertyInfo(Variant::INT, "subd_dist"), "set_subd_dist", "get_subd_dist");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "coll_dist"), "set_coll_dist", "get_coll_dist");
    
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_material", "get_material");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise", PROPERTY_HINT_RESOURCE_TYPE, "FastNoiseLite"), "set_noise", "get_noise");
}

ChunkTree::ChunkTree() {
}

ChunkTree::~ChunkTree() {
}

void ChunkTree::_ready(){

    if (Engine::get_singleton()->is_editor_hint()) {
        set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
        return;
    }
    // new_root(Vector2i(0,0));
}

void ChunkTree::_process(double delta){
    if (divq.empty()){
        // UtilityFunctions::print("empty");
        return;
    }
    DivLeaf queue_leaf = divq.front();
    ChunkLeaf* leaf = queue_leaf.leaf;
    // UtilityFunctions::print(queue_leaf.leaf->get_pos());

    if (queue_leaf.subd == 0){
        leaf->collapse();
    }
    else{
        leaf->subdivide();

        if (leaf->get_level() > 0){
            leaf->update_children(queue_leaf.player_pos);
        }
    }

    divq.pop();
}

void ChunkTree::new_root(Vector2i root_pos){
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    
    root = memnew(ChunkLeaf);
    root->set_pos(root_pos);
    root->set_level(max_level);
    root->set_tree(this);

    add_child(root);
    
    // set parameters for root to pass to each child leaf
    

    
}

void ChunkTree::free_root(){

}

void ChunkTree::add_to_queue(ChunkLeaf* new_leaf, bool subd, Vector2i player_pos){
    DivLeaf new_div_leaf;
    new_div_leaf.leaf = new_leaf;
    new_div_leaf.subd = subd;
    new_div_leaf.player_pos = player_pos;

    divq.push(new_div_leaf);
}

double ChunkTree::get_height(Vector2 coord){
    return noise->get_noise_2d(coord.x, coord.y) * 20.0;
}

