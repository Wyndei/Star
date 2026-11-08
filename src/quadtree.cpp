#include "quadtree.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;


void Leaf::_bind_methods() {
    ClassDB::bind_method(D_METHOD("subdivide"), &Leaf::subdivide);
    ClassDB::bind_method(D_METHOD("collapse"), &Leaf::collapse);
}

Leaf::Leaf() {
}

Leaf::~Leaf() {
}

void Leaf::subdivide(){
    int new_level = level-1;
    // divide leaf into four children

}

void Leaf::collapse(){
}



void QuadTree::_bind_methods() {
    ClassDB::bind_method(D_METHOD("new_root"), &QuadTree::new_root);
    ClassDB::bind_method(D_METHOD("free_root"), &QuadTree::free_root);

}


QuadTree::QuadTree() {
}

QuadTree::~QuadTree() {
}

void QuadTree::new_root(){
    
};

void QuadTree::free_root(){

};