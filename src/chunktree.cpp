#include "chunktree.h"
#include <godot_cpp/core/class_db.hpp>


using namespace godot;


void ChunkLeaf::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_pos", "new_pos"), &ChunkLeaf::set_pos);
    ClassDB::bind_method(D_METHOD("get_pos"), &ChunkLeaf::get_pos);
    ClassDB::bind_method(D_METHOD("get_size"), &ChunkLeaf::get_size);
    ClassDB::bind_method(D_METHOD("set_level, new_level"), &ChunkLeaf::get_level);
    ClassDB::bind_method(D_METHOD("get_level"), &ChunkLeaf::get_level);

    ClassDB::bind_method(D_METHOD("chunk_update", "player_pos"), &ChunkLeaf::chunk_update);
    ClassDB::bind_method(D_METHOD("mesh_chunk"), &ChunkLeaf::mesh_chunk);
    
    ClassDB::bind_method(D_METHOD("set_material", "new_material"), &ChunkLeaf::set_material);
    ClassDB::bind_method(D_METHOD("get_material"), &ChunkLeaf::get_material);

    ClassDB::bind_method(D_METHOD("subdivide"), &ChunkLeaf::subdivide);
    ClassDB::bind_method(D_METHOD("collapse"), &ChunkLeaf::collapse);

    ClassDB::bind_method(D_METHOD("should_subdivide", "player_pos"), &ChunkLeaf::should_subdivide);

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

    int hlf_size = get_size()/2;

    chunk_mesh = memnew(MeshInstance3D); // mesh chunk
    add_child(chunk_mesh);
    chunk_mesh->set_mesh(mesh_chunk());
    chunk_mesh->set_position(Vector3(pos.x-hlf_size, 0.0, pos.y-hlf_size));
    chunk_mesh->set_material_override(terrain_material);

    // test_mesh = memnew(MeshInstance3D); // marker for each chunk
    // Ref<BoxMesh> new_box_mesh;
    // new_box_mesh.instantiate();
    
    // add_child(test_mesh);
    // test_mesh->set_mesh(new_box_mesh);
    // test_mesh->set_position(Vector3(pos.x,0.0,pos.y));
}

void ChunkLeaf::chunk_update(Vector2i player_pos){

    // collapse check
    if (has_children && should_collapse(player_pos)){ // distance check (size * 8)
        collapse(); 
        return;
    } 

    // subdivision checks
    if (level <= 0) return;

    if (!has_children && should_subdivide(player_pos)) subdivide(); // distance check (size * 4)

    if (has_children){ // pass check onto children. if has_children no need to subdivide self
        top_left->chunk_update(player_pos);
        top_right->chunk_update(player_pos);
        bot_left->chunk_update(player_pos);
        bot_right->chunk_update(player_pos);
        return;
    }
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

    top_left->set_min_chunk_size(min_chunk_size);
    top_right->set_min_chunk_size(min_chunk_size);
    bot_left->set_min_chunk_size(min_chunk_size);
    bot_right->set_min_chunk_size(min_chunk_size);

    top_left->set_material(terrain_material);
    top_right->set_material(terrain_material);
    bot_left->set_material(terrain_material);
    bot_right->set_material(terrain_material);

    add_child(top_left);
    add_child(top_right);
    add_child(bot_left);
    add_child(bot_right);

    // hide higher level
    // test_mesh->hide();
    chunk_mesh->hide();
    has_children = true;
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

}

Ref<ArrayMesh> ChunkLeaf::mesh_chunk(){
    //  UtilityFunctions::print("mesh_chunk");

    Ref<ArrayMesh> arr_mesh = memnew(ArrayMesh);

    PackedVector3Array vertices;
    PackedVector3Array normals;
    PackedVector2Array uvs;
    PackedInt32Array indices;

    int resolution = 1 << level;
    int i = 0;
    for(int x = 0; x < min_chunk_size; x++){
        for(int y = 0; y < min_chunk_size; y++){

            vertices.push_back(Vector3((x + 0) * resolution, 0, (y + 0) * resolution)); // create square??
            vertices.push_back(Vector3((x + 1) * resolution, 0, (y + 0) * resolution));
            vertices.push_back(Vector3((x + 1) * resolution, 0, (y + 1) * resolution));
            vertices.push_back(Vector3((x + 0) * resolution, 0, (y + 1) * resolution));
            
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
    
    arr_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);

    return arr_mesh;
}


void ChunkTree::_bind_methods() {
    ClassDB::bind_method(D_METHOD("new_root", "root_pos"), &ChunkTree::new_root);
    ClassDB::bind_method(D_METHOD("free_root"), &ChunkTree::free_root);

    ClassDB::bind_method(D_METHOD("set_max_level", "new_max_level"), &ChunkTree::set_max_level);
    ClassDB::bind_method(D_METHOD("get_max_level"), &ChunkTree::get_max_level);

    ClassDB::bind_method(D_METHOD("set_min_chunk_size", "new_min_chunk_size"), &ChunkTree::set_min_chunk_size);
    ClassDB::bind_method(D_METHOD("get_min_chunk_size"), &ChunkTree::get_min_chunk_size);

    ClassDB::bind_method(D_METHOD("set_material", "new_material"), &ChunkTree::set_material);
    ClassDB::bind_method(D_METHOD("get_material"), &ChunkTree::get_material);

    ClassDB::bind_method(D_METHOD("chunk_updates","player_pos"), &ChunkTree::chunk_updates);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_level"), "set_max_level", "get_max_level");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "min_chunk_size"), "set_min_chunk_size", "get_min_chunk_size");
    
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_material", "get_material");
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
    new_root(Vector2i(0,0));
}

void ChunkTree::new_root(Vector2i root_pos){
    root = memnew(ChunkLeaf);
    add_child(root);
    
    // set parameters for root to pass to each child leaf
    root->set_pos(root_pos);
    root->set_level(max_level);
    root->set_min_chunk_size(min_chunk_size);
    root->set_material(terrain_material);
}

void ChunkTree::free_root(){

}