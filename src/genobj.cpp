#include "genobj.h"
#include <godot_cpp/core/class_db.hpp>


using namespace godot;



void Biome::_bind_methods(){

    ClassDB::bind_method(D_METHOD("set_biome_name", "new_biome"), &Biome::set_biome_name);
    ClassDB::bind_method(D_METHOD("get_biome_name"), &Biome::get_biome_name);

    ClassDB::bind_method(D_METHOD("set_temp", "new_temp"), &Biome::set_temp);
    ClassDB::bind_method(D_METHOD("get_temp"), &Biome::get_temp);

    ClassDB::bind_method(D_METHOD("set_prec", "new_prec"), &Biome::set_prec);
    ClassDB::bind_method(D_METHOD("get_prec"), &Biome::get_prec);

    ClassDB::bind_method(D_METHOD("set_env_objects", "new_env_objects"), &Biome::set_env_objects);
    ClassDB::bind_method(D_METHOD("get_env_objects"), &Biome::get_env_objects);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "biome_name"), "set_biome_name", "get_biome_name");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "temp"), "set_temp", "get_temp");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "prec"), "set_prec", "get_prec");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "env_objects", PROPERTY_HINT_TYPE_STRING, String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":ENVObj"), "set_env_objects", "get_env_objects");
}


void LODMesh::_bind_methods(){
    ClassDB::bind_method(D_METHOD("set_LOD0", "new_LOD0"), &LODMesh::set_LOD0);
    ClassDB::bind_method(D_METHOD("get_LOD0"), &LODMesh::get_LOD0);

    ClassDB::bind_method(D_METHOD("set_LOD1", "new_LOD1"), &LODMesh::set_LOD1);
    ClassDB::bind_method(D_METHOD("get_LOD1"), &LODMesh::get_LOD1);

    ClassDB::bind_method(D_METHOD("set_LOD2", "new_LOD2"), &LODMesh::set_LOD2);
    ClassDB::bind_method(D_METHOD("get_LOD2"), &LODMesh::get_LOD2);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "LOD0", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_LOD0", "get_LOD0");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "LOD1", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_LOD1", "get_LOD1");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "LOD2", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_LOD2", "get_LOD2");

}


void ENVObj::_bind_methods(){
    
    ClassDB::bind_method(D_METHOD("set_lod_mesh", "new_lod_mesh"), &ENVObj::set_lod_mesh);
    ClassDB::bind_method(D_METHOD("get_lod_mesh"), &ENVObj::get_lod_mesh);

    ClassDB::bind_method(D_METHOD("set_weight", "new_weight"), &ENVObj::set_weight);
    ClassDB::bind_method(D_METHOD("get_weight"), &ENVObj::get_weight);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "lod_mesh", PROPERTY_HINT_RESOURCE_TYPE, "LODMesh"), "set_lod_mesh", "get_lod_mesh");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "weight"), "set_weight", "get_weight");

}