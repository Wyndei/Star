#pragma once


#include "quadtree.h"
#include "genobj.h"

#include <queue>
#include <memory>

#include <godot_cpp/core/binder_common.hpp>

#include <godot_cpp/classes/node.hpp>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>

#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>

#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/material.hpp>

#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>

#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>


#include <godot_cpp/classes/fast_noise_lite.hpp>

namespace godot {

class ChunkLeaf;
class ChunkTree;
struct DivLeaf;




class ChunkLeaf : public Leaf{
    GDCLASS(ChunkLeaf, Leaf)


    private:
        int level = 8;

        ChunkTree *tree = nullptr;

        Vector2i pos = Vector2i(0,0);
        
        bool has_children = false;
        bool queued = false;

        MeshInstance3D *test_mesh = nullptr;
        MeshInstance3D *chunk_mesh = nullptr;
        StaticBody3D *chunk_body = nullptr;
        CollisionShape3D *chunk_coll = nullptr;

        std::vector<Ref<MultiMesh>> mm;
        std::vector<MultiMeshInstance3D *> mmi; 

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

    void set_tree(ChunkTree *new_tree){tree = new_tree;}
    ChunkTree* get_tree() {return tree;}

    int get_size();
    
    void chunk_update(Vector2i player_pos);
    void update_children(Vector2i player_pos);

    void generate_mesh();
    void generate_collision();

    void set_mesh(Array surface_array);
    void set_coll(Ref<ConcavePolygonShape3D> new_coll);


    bool should_subdivide(Vector2i player_pos);
    bool should_collapse(Vector2i player_pos);

    void add_to_queue(bool subd, Vector2i player_pos);

    void subdivide();
    void collapse();

    void generate_visual();
    void start_objects();
    void generate_objects();
    void set_chunk_multimesh(Ref<MultiMesh> new_mm, int i);

    int get_object_index(TypedArray<Ref<ENVObj>> env_objects, Vector2 pos);
};


struct DivLeaf {
    ChunkLeaf *leaf;
    bool subd = false;
    Vector2i player_pos;

    // bool operator<(const DivLeaf& other) const;
};

class ChunkTree : public QuadTree {
    GDCLASS(ChunkTree, QuadTree)

    private:

        ChunkLeaf *root;
        std::queue<std::shared_ptr<DivLeaf>> divq;

        int world_seed = 0;

        int max_level = 8;
        int min_chunk_size = 8; // in meters
        int subd_dist = 4;
        int coll_dist = 6;

        Ref<FastNoiseLite> noise;
        Ref<FastNoiseLite> mountain_noise;
        Ref<Material> terrain_material;
        // (largest_chunk_size = smallest_chunk_size * highest_level) in meters

        Ref<Biome> biome; // test


    protected:
        static void _bind_methods();


    public:
        ChunkTree();
        ~ChunkTree();

        void _ready() override;
        void _process(double delta) override;

        void new_root(Vector2i root_pos);
        void free_root();

        void add_to_queue(ChunkLeaf *new_leaf, bool subd, Vector2i player_pos);

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

        void set_noise(const Ref<FastNoiseLite> &new_noise){noise = new_noise;}
        Ref<FastNoiseLite> get_noise(){return noise;}
        void set_mountain_noise(const Ref<FastNoiseLite> &new_noise){mountain_noise = new_noise;}
        Ref<FastNoiseLite> get_mountain_noise(){return mountain_noise;}

        void set_biome(const Ref<Biome> &new_biome){biome = new_biome;}
        Ref<Biome> get_biome(){return biome;}

        float get_height(Vector2 coord);
        Vector2 get_gradient(Vector2 coord, float epsilon);

        void set_world_seed(const int new_world_seed){world_seed = new_world_seed;}
        int get_world_seed(){return world_seed;}
};


} // namespace godot}
