#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/vector2i.hpp>

namespace godot {

class Leaf : public Node {
	GDCLASS(Leaf, Node)

    private:

        int level = 5;

    protected:
        static void _bind_methods();

    public:
        Leaf();
        ~Leaf();


        void subdivide();
        void collapse();
};


class QuadTree : public Node {
    GDCLASS(QuadTree, Node)


    private:

        int max_level = 5;

        // (largest_chunk_size = smallest_chunk_size * highest_level) in meters


    protected:
        static void _bind_methods();


    public:
        QuadTree();
        ~QuadTree();

        void new_root();
        void free_root();
};


} // namespace godot}
