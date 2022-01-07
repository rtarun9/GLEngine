#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <iostream>
#include <chrono>

#include "../include/shader.hpp"
#include "../include/camera.hpp"

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

	// Prepare for rendering
	float vertices[] =
	{
		  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	uint32_t vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uint32_t vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	// Load texture
	int width = 0;
	int height = 0;
	int num_channels = 0;
	unsigned char* data = stbi_load("../assets/images/container.jpg", &width, &height, &num_channels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture";
	}

	uint32_t texture = 0;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	Shader shader("../shaders/test_vertex.glsl", "../shaders/test_fragment.glsl");

	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window))
	{
		g_current_frame_time = g_clock.now();
		g_delta_time = (g_current_frame_time - g_previous_frame_time).count() * 1e-9;
		g_previous_frame_time = g_current_frame_time;

		glfwPollEvents();
		process_input(window);
		
		glm::mat4 model_mat = glm::mat4(1.0f);
		//model_mat = glm::rotate(model_mat, glm::radians(static_cast<float>(sinf(glfwGetTime() / 100.0f))), glm::vec3(1.0f, 0.0f, 1.0f));

		glm::mat4 view_mat = g_camera.get_view_mat();
		glm::mat4 projection_mat = glm::perspective(glm::radians(g_camera.get_zoom()), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.set_float("u_time", glfwGetTime());
		shader.set_mat4("model_mat", model_mat);
		shader.set_mat4("view_mat", view_mat);
		shader.set_mat4("projection_mat", projection_mat);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}