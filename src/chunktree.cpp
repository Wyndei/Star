#include "chunktree.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;


void ChunkLeaf::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_pos", "new_pos"), &ChunkLeaf::set_pos);
    ClassDB::bind_method(D_METHOD("get_pos"), &ChunkLeaf::get_pos);
    ClassDB::bind_method(D_METHOD("get_size"), &ChunkLeaf::get_size);

    ClassDB::bind_method(D_METHOD("subdivide"), &ChunkLeaf::subdivide);
    ClassDB::bind_method(D_METHOD("collapse"), &ChunkLeaf::collapse);

}

ChunkLeaf::ChunkLeaf() {
}

ChunkLeaf::~ChunkLeaf() {
}


void ChunkLeaf::subdivide(){
    int chunk_size = get_size();
    int new_level = level-1;
    int new_size = new_level;
    // divide leaf into four children

    int hlf_size = chunk_size/2;
    int qtr_size = chunk_size/4;

    // instantiate children

    // Vector2i(hlf_size - qtr_size, hlf_size - qtr_size); // top left
    // Vector2i(hlf_size + qtr_size, hlf_size - qtr_size); // top right
    // Vector2i(hlf_size - qtr_size, hlf_size + qtr_size); // bot left
    // Vector2i(hlf_size + qtr_size, hlf_size + qtr_size); // bot right

}


void ChunkLeaf::collapse(){
    // delete all children

    // unhide lower subdiv terrain

}



void ChunkTree::_bind_methods() {
    ClassDB::bind_method(D_METHOD("new_root"), &ChunkTree::new_root);
    ClassDB::bind_method(D_METHOD("free_root"), &ChunkTree::free_root);

}

ChunkTree::ChunkTree() {
}

ChunkTree::~ChunkTree() {
}

void ChunkTree::new_root(){

};

void ChunkTree::free_root(){

};