#pragma once

#include <godot_cpp/core/binder_common.hpp>

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/mesh.hpp>

namespace godot{

class Biome;
class LODMesh;
class ENVObj;


class Biome : public Resource{
    GDCLASS(Biome, Resource)

    private:
        String biome_name = "";

        float temp = 0.5;
        float prec = 0.5;

        TypedArray<Ref<ENVObj>> env_objects;

    protected:
        static void _bind_methods();

    public:
        Biome(){};
        ~Biome(){};

        void set_biome_name(const String &new_name) {biome_name = new_name;}
        String get_biome_name() const {return biome_name;}

        void set_temp(float new_temp) {temp = new_temp;}
        float get_temp() const {return temp;}

        void set_prec(float new_prec) {prec = new_prec;}
        float get_prec() const {return prec;}

        void set_env_objects(const TypedArray<Ref<ENVObj>> &new_env_objects) {env_objects = new_env_objects;}
        TypedArray<Ref<ENVObj>> get_env_objects() const {return env_objects;}

};

class LODMesh : public Resource{
    GDCLASS(LODMesh, Resource)

    private:
        Ref<Mesh> LOD0;
        Ref<Mesh> LOD1;
        Ref<Mesh> LOD2;

    protected:
        static void _bind_methods();


    public:
        LODMesh(){};
        ~LODMesh(){};
    
    void set_LOD0(Ref<Mesh> new_LOD0) {LOD0 = new_LOD0;}
    Ref<Mesh> get_LOD0() const {return LOD0;}

    void set_LOD1(Ref<Mesh> new_LOD1) {LOD1 = new_LOD1;}
    Ref<Mesh> get_LOD1() const {return LOD1;}

    void set_LOD2(Ref<Mesh> new_LOD2) {LOD2 = new_LOD2;}
    Ref<Mesh> get_LOD2() const {return LOD2;}

};

class ENVObj : public Resource{
    GDCLASS(ENVObj, Resource)

    private:
        Ref<LODMesh> mesh;
        int weight = 10.0;

    protected:
        static void _bind_methods();

    public:
        ENVObj(){};
        ~ENVObj(){};

    void set_lod_mesh(Ref<LODMesh> new_lod_mesh) {mesh = new_lod_mesh;}
    Ref<LODMesh> get_lod_mesh() const {return mesh;}

    void set_weight(int new_weight) {weight = new_weight;}
    int get_weight() const {return weight;}

};

}// namespace godot}