#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <iostream>
#include <chrono>

#include "../include/ui_manager.hpp"
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

static bool g_use_mouse = true;
Camera g_camera = Camera();

GLFWwindow* create_window(const char *window_title, int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void process_mouse(GLFWwindow* window, double xpos, double ypos);
void process_scroll(GLFWwindow* window, double xoffset, double yoffset);

struct OffscreenRT
{
	OffscreenRT();

	uint32_t fbo;
	uint32_t color_attachment;
	uint32_t depth_attachment;

	uint32_t vao;
	uint32_t vbo;
};

int main()
{
	GLFWwindow* window = create_window("GLEngine", SCREEN_WIDTH, SCREEN_HEIGHT);
	UIManager ui_manager(window);

	// Depth map for shadow mapping
	uint32_t depth_fbo;
	glGenFramebuffers(1, &depth_fbo);

	constexpr int SHADOW_DEPTH_MAP_DIMENSION = 1024;
	uint32_t depth_map;
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_DEPTH_MAP_DIMENSION, SHADOW_DEPTH_MAP_DIMENSION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float near_plane = 0.1f;
	float far_plane = 1000.0f;
	glm::mat4 light_projection_mat = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
	glm::mat4 light_view_mat = glm::lookAt(glm::vec3(-2.0f, 100.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 light_transform_mat = light_projection_mat * light_view_mat;

	// Frame resources
	Model sponza("../assets/models/sponza-gltf/sponza.gltf");
	Model cube("../assets/models/cube/Cube.gltf");
	
	Shader light_shader("../shaders/light_vertex.glsl", "../shaders/light_fragment.glsl");
	Shader shader("../shaders/test_vertex.glsl", "../shaders/test_fragment.glsl");
	Shader offscreen_fb_shader("../shaders/offscreen_vertex.glsl", "../shaders/offscreen_fragment.glsl");
	Shader shadow_shader("../shaders/shadow_vertex.glsl", "../shaders/shadow_fragment.glsl");

	OffscreenRT offscreen_rt{};

	ImVec4 clear_color = ImVec4(0.1f, 0.6f, 0.8f, 1.0f);

	glm::mat4 model_mat = glm::mat4(1.0f);
	model_mat = glm::scale(model_mat, glm::vec3(0.1f));
	glm::mat4 view_mat = g_camera.get_view_mat();
	glm::mat4 projection_mat = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

	glm::vec3 light_position = glm::vec3(0.0f, 10.0f, 0.0f);
	glm::vec3 light_color = glm::vec3(1.0f);


	while (!glfwWindowShouldClose(window))
	{
		g_current_frame_time = g_clock.now();
		g_delta_time = static_cast<float>((g_current_frame_time - g_previous_frame_time).count() * 1e-9);
		g_previous_frame_time = g_current_frame_time;

		glfwPollEvents();
		process_input(window);
		
		glBindFramebuffer(GL_FRAMEBUFFER, offscreen_rt.fbo);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ui_manager.render(clear_color, g_camera.m_movement_speed, light_position, light_color);

		// draw scene
		
		// draw light sources
		view_mat = g_camera.get_view_mat();

		light_shader.use();
		model_mat = glm::mat4(1.0f);
		model_mat = glm::translate(model_mat, light_position);
		light_shader.set_mat4("model_mat", model_mat);
		light_shader.set_mat4("view_mat", view_mat);
		light_shader.set_mat4("projection_mat", projection_mat);
		light_shader.set_vec3f("light_color", light_color);

		cube.draw(light_shader);

		// draw other models for scene
		model_mat = glm::mat4(1.0f);

		model_mat = glm::scale(model_mat, glm::vec3(0.1f));
		shader.use();
		shader.set_mat4("model_mat", model_mat);
		shader.set_mat4("view_mat", view_mat);
		shader.set_mat4("projection_mat", projection_mat);
		shader.set_vec3f("light_pos", light_position);
		shader.set_vec3f("camera_pos", g_camera.m_position);
		shader.set_vec3f("light_color", light_color);
		sponza.draw(shader);

		// render to default FBO

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		offscreen_fb_shader.use();
		glBindTexture(GL_TEXTURE_2D, offscreen_rt.color_attachment);

		glBindVertexArray(offscreen_rt.vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		ui_manager.present();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

GLFWwindow* create_window(const char* window_title, int width, int height)
{
	// Initialize and configure GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW window";
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD";
		exit(-1);
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, process_mouse);
	glfwSetScrollCallback(window, process_scroll);

	return window;
}

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

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		g_use_mouse = false;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		g_use_mouse = true;
	}
}

void process_mouse(GLFWwindow* window, double xpos, double ypos)
{
	if (g_first_mouse)
	{
		g_last_x = static_cast<float>(xpos);
		g_last_y = static_cast<float>(ypos);

		g_first_mouse = false;
	}

	float xoffset = static_cast<float>(xpos - g_last_x);
	float yoffset = static_cast<float>(g_last_y - ypos);

	g_last_x = static_cast<float>(xpos);
	g_last_y = static_cast<float>(ypos);

	if (g_use_mouse)
	{
		g_camera.process_mouse(xoffset, yoffset);
	}
}

void process_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	g_camera.process_scroll(static_cast<float>(yoffset));
}

OffscreenRT::OffscreenRT()
	: fbo(-1), color_attachment(-1), depth_attachment(-1)
{
	// set up offscreen vbo and vao
	float fbo_vertices[] =
	{
	   -1.0f, 1.0f, 0.0f, 1.0f,
	   -1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

	   -1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// color attachment
	glGenTextures(1, &color_attachment);
	glBindTexture(GL_TEXTURE_2D, color_attachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attach color attachment to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment, 0);

	// depth attachment
	glGenTextures(1, &depth_attachment);
	glBindTexture(GL_TEXTURE_2D, depth_attachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attach depth attachment to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment, 0);

	// check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not complete : " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << "\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
