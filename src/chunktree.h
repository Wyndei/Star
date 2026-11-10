#pragma once

#include "quadtree.h"

#include <godot_cpp/core/binder_common.hpp>

#include <godot_cpp/classes/node.hpp>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/material.hpp>

#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>

#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>

namespace godot {


class ChunkLeaf : public Leaf{
    GDCLASS(ChunkLeaf, Leaf)


    private:
        int level = 8;
        int min_chunk_size = 8;
        int subd_dist = 4;
        int coll_dist = 6;

        Vector2i pos = Vector2i(0,0);
        
        bool has_children = false;

        MeshInstance3D *test_mesh;
        MeshInstance3D *chunk_mesh;
        StaticBody3D *chunk_body;
        CollisionShape3D *chunk_coll;


        ChunkLeaf *top_left;
        ChunkLeaf *top_right;
        ChunkLeaf *bot_left;
        ChunkLeaf *bot_right;

        Ref<Material> terrain_material;

    protected:
        static void _bind_methods();


    public:

    ChunkLeaf();
    ~ChunkLeaf();

    void _ready() override;

    void set_pos(const Vector2i new_pos){pos = new_pos;}
    Vector2i get_pos() {return pos;}

    void set_level(const int new_level){level = new_level;}
    int get_level() {return level;}

    void set_min_chunk_size(const int new_min){min_chunk_size = new_min;}
    int get_min_chunk_size(){return min_chunk_size;}

    void set_subd_dist(const int new_subd_dist){subd_dist = new_subd_dist;}
    int get_subd_dist() {return subd_dist;}

    void set_coll_dist(const int new_coll_dist){coll_dist = new_coll_dist;}
    int get_coll_dist() {return coll_dist;}

    void set_material(const Ref<Material> &new_material){terrain_material = new_material;}
    Ref<Material> get_material(){return terrain_material;}

    int get_size(){return min_chunk_size * pow(2,level);}
    
    void chunk_update(Vector2i player_pos);
    
    void generate_mesh();
    void generate_collision();

    void set_mesh(Array surface_array);
    void set_coll(Ref<ConcavePolygonShape3D> new_coll);

    bool should_subdivide(Vector2i player_pos){if (player_pos.distance_to(pos) < get_size() * subd_dist) return true; else {return false;};}
    bool should_collapse(Vector2i player_pos){if (player_pos.distance_to(pos) > get_size() * coll_dist) return true; else {return false;};}

    void subdivide();
    void collapse();

    void test_visual();
};



class ChunkTree : public QuadTree {
    GDCLASS(ChunkTree, QuadTree)

    private:

        ChunkLeaf *root;

        int max_level = 8;
        int min_chunk_size = 8; // in meters
        int subd_dist = 4;
        int coll_dist = 6;

        Ref<Material> terrain_material;
        // (largest_chunk_size = smallest_chunk_size * highest_level) in meters



    protected:
        static void _bind_methods();


    public:
        ChunkTree();
        ~ChunkTree();

        void _ready() override;

        void new_root(Vector2i root_pos);
        void free_root();

        void chunk_updates(Vector2i player_pos){root->chunk_update(player_pos);}

        void set_max_level(const int new_level){max_level = new_level;}
        int get_max_level() {return max_level;}

        void set_min_chunk_size(const int new_min){min_chunk_size = new_min;}
        int get_min_chunk_size(){return min_chunk_size;}

        void set_subd_dist(const int new_subd_dist){subd_dist = new_subd_dist;}
        int get_subd_dist() {return subd_dist;}

        void set_coll_dist(const int new_coll_dist){coll_dist = new_coll_dist;}
        int get_coll_dist() {return coll_dist;}

        void set_material(const Ref<Material> &new_material){terrain_material = new_material;}
        Ref<Material> get_material(){return terrain_material;}
};


} // namespace godot}
