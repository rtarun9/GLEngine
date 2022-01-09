#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <iostream>
#include <chrono>

#include "../include/shader.hpp"
#include "../include/camera.hpp"
#include "../include/model.hpp"

static constexpr int SCREEN_WIDTH = 1920;
static constexpr int SCREEN_HEIGHT = 1080;

static std::chrono::high_resolution_clock g_clock;
static std::chrono::high_resolution_clock::time_point g_current_frame_time;
static std::chrono::high_resolution_clock::time_point g_previous_frame_time;
static float g_delta_time = 0.0f;

static float g_last_x = SCREEN_WIDTH / 2.0f;
static float g_last_y = SCREEN_HEIGHT / 2.0f;
static bool g_first_mouse = true;

Camera g_camera = Camera();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		g_camera.process_keyboard(g_delta_time, Directions::Forward);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		g_camera.process_keyboard(g_delta_time, Directions::Left);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		g_camera.process_keyboard(g_delta_time, Directions::Right);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		g_camera.process_keyboard(g_delta_time, Directions::Backward);
	}
}

void process_mouse(GLFWwindow* window, double xpos, double ypos)
{
	if (g_first_mouse)
	{
		g_last_x = xpos;
		g_last_y = ypos;

		g_first_mouse = false;
	}

	float xoffset = xpos - g_last_x;
	float yoffset = g_last_y - ypos;

	g_last_x = xpos;
	g_last_y = ypos;

	g_camera.process_mouse(xoffset, yoffset);
}

void process_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	g_camera.process_scroll(yoffset);
}

int main()
{
	// Initialize and configure GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GLEngine", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW window";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD";
		return -1;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, process_mouse);
	glfwSetScrollCallback(window, process_scroll);

	Shader shader("../shaders/test_vertex.glsl", "../shaders/test_fragment.glsl");
	Shader light_shader("../shaders/light_vertex.glsl", "../shaders/light_fragment.glsl");

	Model sponza("../assets/models/sponza/sponza.obj");
	Model cube("../assets/models/cube/cube.obj");



	glm::vec3 light_position = glm::vec3(0.0f, 4.0f, 0.0f);
	glm::vec3 light_color = glm::vec3(1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
		glEnable(GL_CULL_FACE);

		g_current_frame_time = g_clock.now();
		g_delta_time = (g_current_frame_time - g_previous_frame_time).count() * 1e-9;
		g_previous_frame_time = g_current_frame_time;

		glfwPollEvents();
		process_input(window);
		glm::mat4 model_mat = glm::mat4(1.0f);

		//model_mat = glm::rotate(model_mat, glm::radians(static_cast<float>(sinf(glfwGetTime() / 100.0f))), glm::vec3(1.0f, 0.0f, 1.0f));

		glm::mat4 view_mat = g_camera.get_view_mat();
		glm::mat4 projection_mat = glm::perspective(glm::radians(g_camera.get_zoom()), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
		model_mat = glm::translate(model_mat, light_position);
		model_mat = glm::scale(model_mat, glm::vec3(0.1f));
		glClearColor(0.4f, 0.5f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		light_shader.use();
		light_shader.set_mat4("model_mat", model_mat);
		light_shader.set_mat4("view_mat", view_mat);
		light_shader.set_mat4("projection_mat", projection_mat);

		light_shader.set_vec3f("light_color", light_color);
		cube.draw(light_shader);

		model_mat = glm::mat4(1.0f);
		model_mat = glm::scale(model_mat, glm::vec3(0.01f));
		shader.use();
		shader.set_vec3f("light_position", light_position);
		shader.set_vec3f("camera_position", g_camera.m_position);
		shader.set_vec3f("light_color", light_color);

		shader.set_mat4("model_mat", model_mat);
		shader.set_mat4("view_mat", view_mat);
		shader.set_mat4("projection_mat", projection_mat);

		sponza.draw(shader);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}