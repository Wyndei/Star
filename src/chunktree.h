#pragma once

#include "quadtree.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/vector2i.hpp>

namespace godot {


class ChunkLeaf : public Leaf{
    GDCLASS(ChunkLeaf, Leaf)


    private:
        int level;
        int min_chunk_size;
        Vector2i pos;

    protected:
        static void _bind_methods();


    public:

    ChunkLeaf();
    ~ChunkLeaf();

    void set_pos(const Vector2i new_pos){pos = new_pos;}
    Vector2i get_pos() {return pos;}
    int get_size(){return min_chunk_size * level;}

    void subdivide();
    void collapse();

};



class ChunkTree : public QuadTree {
    GDCLASS(ChunkTree, QuadTree)



    private:

        int min_chunk_size = 16; // in meters

        // (largest_chunk_size = smallest_chunk_size * highest_level) in meters


    protected:
        static void _bind_methods();


    public:

        ChunkTree();
        ~ChunkTree();

        void new_root();
        void free_root();

};


} // namespace godot}
