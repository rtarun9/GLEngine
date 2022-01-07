#include "../include/shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vs_path, const char* fs_path)
	: m_program(0), m_vertex_shader(0), m_fragment_shader(0)
{
	std::ifstream vs_file(vs_path);
	if (!vs_file.is_open())
	{
		std::cout << "Failed to open file : " << vs_path << '\n';
		return;
	}
	
	std::ifstream fs_file(fs_path);
	if (!fs_file.is_open())
	{
		std::cout << "Failed to open file : " << fs_path << '\n';
		return;
	}

	std::stringstream vs_stream;
	vs_stream << vs_file.rdbuf();

	std::stringstream fs_stream;
	fs_stream << fs_file.rdbuf();

	std::string vs_source = vs_stream.str();
	std::string fs_source = fs_stream.str();

	const char* vs_source_code = vs_source.c_str();
	const char* fs_source_code = fs_source.c_str();

	m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vertex_shader, 1, &vs_source_code, nullptr);
	glCompileShader(m_vertex_shader);

	// Check for compilation errors of VS
	{
		int success = 0;
		char info_log[512] = {};

		glGetShaderiv(m_vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(m_vertex_shader, 512, nullptr, info_log);
			std::cout << "VS ERROR : " << info_log << '\n';
			std::cout << "Path : " << vs_path;
		}
	}

	m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_fragment_shader, 1, &fs_source_code, nullptr);
	glCompileShader(m_fragment_shader);

	// Check for compilation errors of FS
	{
		int success = 0;
		char info_log[512] = {};

		glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(m_fragment_shader, 512, nullptr, info_log);
			std::cout << "VS ERROR : " << info_log << '\n';
			std::cout << "Path : " << fs_path;
		}
	}

	m_program = glCreateProgram();
	glAttachShader(m_program, m_vertex_shader);
	glAttachShader(m_program, m_fragment_shader);
	glLinkProgram(m_program);

	// Check for compilation errors of shader program
	{
		int success = 0;
		char info_log[512] = {};

		glGetProgramiv(m_program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(m_program, 512, nullptr, info_log);
			std::cout << "SHADER PROGRAM ERROR : " << info_log << '\n';
			std::cout << "Path of vs : " << vs_path << "\n";
			std::cout << "Path of fs : " << fs_path << "\n";
		}
	}

	glDeleteShader(m_vertex_shader);
	glDeleteShader(m_fragment_shader);
}

void Shader::use()
{
	glUseProgram(m_program);
}

void Shader::set_int(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(m_program, name), value);
}

void Shader::set_float(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(m_program, name), value);
}

void Shader::set_vec3f(const char* name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(m_program, name), 1, &value[0]);
}

void Shader::set_vec3f(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(m_program, name), x, y, z);
}

void Shader::set_mat4(const char* name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_program, name), 1, GL_FALSE, &mat[0][0]);
}