#pragma once

#include "quadtree.h"

#include <godot_cpp/classes/node.hpp>

#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

namespace godot {


class ChunkLeaf : public Leaf{
    GDCLASS(ChunkLeaf, Leaf)


    private:
        int level = 6;
        int min_chunk_size = 8;
        Vector2i pos = Vector2i(0,0);
        
        bool has_children = false;

        MeshInstance3D *test_mesh;

        ChunkLeaf *top_left;
        ChunkLeaf *top_right;
        ChunkLeaf *bot_left;
        ChunkLeaf *bot_right;

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

    int get_size(){return min_chunk_size * pow(2,level);}

    void chunk_update(Vector2i player_pos);

    bool should_subdivide(Vector2i player_pos){if (player_pos.distance_to(pos) < get_size() * 4) return true; else {return false;};}

    bool should_collapse(Vector2i player_pos){if (player_pos.distance_to(pos) > get_size() * 6) return true; else {return false;};}

    void subdivide();
    void collapse();

    void test_visual();
};



class ChunkTree : public QuadTree {
    GDCLASS(ChunkTree, QuadTree)



    private:

        ChunkLeaf *root;

        // int min_chunk_size = 8; // in meters

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
};


} // namespace godot}
