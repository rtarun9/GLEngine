#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstdint>

class Shader
{
public:
	Shader(const char* vs_path, const char* fs_path);

	void use();

	void set_int(const char* name, int value) const;
	void set_bool(const char* name, bool value) const;
	void set_float(const char* name, float value) const;
	void set_vec3f(const char* name, const glm::vec3& value) const;
	void set_vec3f(const char* name, float x, float y, float z) const;
	void set_mat4(const char* name, const glm::mat4& mat) const;

private:
	uint32_t m_program;
	uint32_t m_vertex_shader;
	uint32_t m_fragment_shader;
};