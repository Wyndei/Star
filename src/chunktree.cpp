#include "chunktree.h"
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>

#include <godot_cpp/classes/box_mesh.hpp>

#include <godot_cpp/variant/vector3.hpp>

using namespace godot;


void ChunkLeaf::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_pos", "new_pos"), &ChunkLeaf::set_pos);
    ClassDB::bind_method(D_METHOD("get_pos"), &ChunkLeaf::get_pos);
    ClassDB::bind_method(D_METHOD("get_size"), &ChunkLeaf::get_size);
    ClassDB::bind_method(D_METHOD("set_level, new_level"), &ChunkLeaf::get_level);
    ClassDB::bind_method(D_METHOD("get_level"), &ChunkLeaf::get_level);

    ClassDB::bind_method(D_METHOD("chunk_update", "player_pos"), &ChunkLeaf::chunk_update);
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
    test_visual();
}

void ChunkLeaf::test_visual(){
    for(int x = 0; x < min_chunk_size/2; x++){
        for(int y = 0; y < min_chunk_size/2; y++){
            test_mesh = memnew(MeshInstance3D);
            Ref<BoxMesh> new_mesh;
            new_mesh.instantiate();
            
            add_child(test_mesh);
            test_mesh->set_mesh(new_mesh);
            test_mesh->set_position(Vector3(pos.x + (x * pow(2, level)*4), 0.0 , pos.y + (y * pow(2, level)*4)));
        }
    }
    // test_mesh = memnew(MeshInstance3D);
    // Ref<BoxMesh> new_mesh;
    // new_mesh.instantiate();
    
    // add_child(test_mesh);
    // test_mesh->set_mesh(new_mesh);
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

    int hlf_size = chunk_size/2;
    int qtr_size = chunk_size/4;

    // instantiate children

    top_left  = memnew(ChunkLeaf);
    top_right = memnew(ChunkLeaf);
    bot_left  = memnew(ChunkLeaf);
    bot_right = memnew(ChunkLeaf);
    
    top_left->set_pos(pos + Vector2i(-hlf_size, hlf_size));
    top_right->set_pos(pos + Vector2i(hlf_size, hlf_size));
    bot_left->set_pos(pos + Vector2i(-hlf_size, -hlf_size));
    bot_right->set_pos(pos + Vector2i(hlf_size, -hlf_size));

    top_left->set_level(new_level);
    top_right->set_level(new_level);
    bot_left->set_level(new_level);
    bot_right->set_level(new_level);

    add_child(top_left);
    add_child(top_right);
    add_child(bot_left);
    add_child(bot_right);

    // hide higher level
    test_mesh->hide();
    has_children = true;
}

void ChunkLeaf::collapse(){
    // UtilityFunctions::print("coll");
    // unhide lower subdiv terrain
    test_mesh->show();

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



void ChunkTree::_bind_methods() {
    ClassDB::bind_method(D_METHOD("new_root", "root_pos"), &ChunkTree::new_root);
    ClassDB::bind_method(D_METHOD("free_root"), &ChunkTree::free_root);

    ClassDB::bind_method(D_METHOD("chunk_updates","player_pos"), &ChunkTree::chunk_updates);

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
    root->set_pos(root_pos);
}

void ChunkTree::free_root(){

}