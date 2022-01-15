#include "../include/model.hpp"

#include <stb_image.h>
#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <GL/glext.h>

unsigned int texture_from_file(const char *path, const std::string &directory, bool gamma = true)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int width, height, num_components;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &num_components, 0);
    if (data)
    {
        GLenum format;
        if (!gamma)
        {
            if (num_components == 1)
            {
                format = GL_RED;
            }
            else if (num_components == 3)
            {
                format = GL_RGB;
            }
            else if (num_components == 4)
            {
                format = GL_RGBA;
            }
        }
        else
        {
            if (num_components == 1)
            {
                format = GL_RED;
            }
            else if (num_components == 3)
            {
                format = GL_SRGB;
            }
            else if (num_components == 4)
            {
                format = GL_SRGB_ALPHA;
            }
        }
      
        glBindTexture(GL_TEXTURE_2D, texture_id);

        float aniso = 0.0f;
        
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return texture_id;
}

Model::Model(const char *path)
{
    load_model(path);
}

void Model::draw(Shader& shader)
{
    for (uint32_t i = 0; i < m_meshes.size(); i++)
    {
        m_meshes[i].draw(shader);
    }
}

void Model::load_model(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Cannot load model with path : " << path << '\n';
        return;
    }

    m_directory = path.substr(0, path.find_last_of('/'));

    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene* scene)
{
    // process all meshes of node
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(process_mesh(mesh, scene));
    }

    // process all children of current node
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        // NOTE : ASSIMP CAN HAVE 8 TEX COORDS PER VERTEX, BUT USING ONLY FIRST FOR NOW
        if (mesh->mTextureCoords[0])
        {
            vertex.tex_coords.x = mesh->mTextureCoords[0][i].x;
            vertex.tex_coords.y = mesh->mTextureCoords[0][i].y;

            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;

            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        }
        else
        {
            vertex.tex_coords.x = 0;
            vertex.tex_coords.y = 0;

            vertex.tangent.x = 0;
            vertex.tangent.y = 0;
            vertex.tangent.z = 0;

            vertex.bitangent.x = 0;
            vertex.bitangent.y = 0;
            vertex.bitangent.z = 0;
        }

        vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuse_maps = load_material_texture(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

        std::vector<Texture> specular_maps = load_material_texture(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
    
        std::vector<Texture> normal_maps = load_material_texture(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

        std::vector<Texture> height_maps = load_material_texture(material, aiTextureType_HEIGHT, "texture_height");
        textures.insert(textures.end(), height_maps.begin(), height_maps.end());

    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_texture(aiMaterial *material, aiTextureType type, std::string type_name)
{
    std::vector<Texture> textures;

    for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);

        bool already_added = false;
        for (uint32_t j = 0; j < m_textures_loaded.size(); j++)
        {
            if (std::strcmp(m_textures_loaded[i].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(m_textures_loaded[j]);
                already_added = true;
                break;
            }
        }

        if (already_added)
        {
            continue;
        }
        
        Texture texture;
        if (type_name == "texture_normal" || type_name == "texture_height")
        {
            texture.texture_id = texture_from_file(str.C_Str(), m_directory, false);
        }
        else
        {
            texture.texture_id = texture_from_file(str.C_Str(), m_directory, false);
        }

        texture.type = type_name;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }

    return textures;
}
