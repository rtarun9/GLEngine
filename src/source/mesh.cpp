#include "../include/mesh.hpp"

#include <glad/glad.h>

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, std::vector<Texture>& textures)
{
    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;

    setup_mesh();
}

void Mesh::draw(Shader& shader)
{
    uint32_t diffuse_num = 1;
    uint32_t specular_num = 1;
    uint32_t normal_num = 1;
    uint32_t height_num = 1;

    for (uint32_t i = 0; i < m_textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string texture_type = m_textures[i].type;

        if (texture_type == "texture_diffuse")
        {
            number = std::to_string(diffuse_num++);
        }
        else if (texture_type == "texture_specular")
        {
            number = std::to_string(specular_num++);
        }
        else if (texture_type == "texture_normal")
        {
            number = std::to_string(normal_num++);
        }
        else if (texture_type == "texture_height")
        {
            number = std::to_string(height_num++);
        }

        shader.set_float(("material." + texture_type + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].texture_id);
    }


    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setup_mesh()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tex_coords)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, bitangent)));

    glBindVertexArray(0);
}