#pragma once

#include "shader.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tex_coords;
};

struct Texture
{
	uint32_t texture_id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, std::vector<Texture>& textures);

	void draw(Shader& shader);

	void setup_mesh();

public:
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	std::vector<Texture> m_textures;

private:
	uint32_t m_vao;
	uint32_t m_vbo;
	uint32_t m_ebo;
};