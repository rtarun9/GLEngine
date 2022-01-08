#pragma once

#include "shader.hpp"
#include "mesh.hpp"

#include <assimp/scene.h>

class Model
{
public:
    Model(const char *path);
    void draw(Shader& shader);

    void load_model(const std::string& path);
    
    void process_node(aiNode *node, const aiScene* scene);

    // Translate aiMesh into a Mesh object
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> load_material_texture(aiMaterial *material, aiTextureType type, std::string type_name);

private:
    std::vector<Mesh> m_meshes;
    std::string m_directory;

    std::vector<Texture> m_textures_loaded;
};