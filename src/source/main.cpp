#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

static bool g_use_mouse = true;
Camera g_camera = Camera();


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void process_mouse(GLFWwindow* window, double xpos, double ypos);
void process_scroll(GLFWwindow* window, double xoffset, double yoffset);

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

	// Set up imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImVec4 clear_color = ImVec4(0.1f, 0.6f, 0.8f, 1.0f);

	// Setup frame resources
	Shader shader("../shaders/test_vertex.glsl", "../shaders/test_fragment.glsl");
	Shader depth_shader("../shaders/shadow_vertex.glsl", "../shaders/shadow_fragment.glsl");

	Model sponza("../assets/models/sponza-gltf/Sponza.gltf");
	Model cube("../assets/models/cube/cube.obj");

	glm::vec3 light_color = glm::vec3(1.0f, 0.9f, 0.9f);
	glm::vec3 directional_light_dir = glm::vec3(-5.0f, -10.1f, -2.0f);
	glm::vec3 cube_position = glm::vec3(0.0f, 10.0f, 0.0f);

	// Setup up resources for shadow mapping
	uint32_t depth_map_fbo;
	glGenFramebuffers(1, &depth_map_fbo);

	uint32_t depth_map_dimension = 2048;

	uint32_t depth_map;
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depth_map_dimension, depth_map_dimension, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float clamp_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clamp_color);
	
	// Attach depth map to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window))
	{
		g_current_frame_time = g_clock.now();
		g_delta_time = (g_current_frame_time - g_previous_frame_time).count() * 1e-9;
		g_previous_frame_time = g_current_frame_time;

		glfwPollEvents();
		process_input(window);

		// draw ImGui stuff
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Scene settings");
			ImGui::ColorEdit3("clear_color", (float*)&clear_color);
			ImGui::SliderFloat3("directional_light_pos", &directional_light_dir[0], -10.0f, 10.0f, "%f", 1.0f);
			ImGui::SliderFloat3("light_color", &light_color[0], 0.0f, 1.0f, "%f", 1.0f);
			ImGui::SliderFloat3("cube_position", &cube_position[0], -50.0f, 50.0f, "%f", 1.0f);

			ImGui::End();
			ImGui::Render();

		}

		// draw to depth map
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

			
		glm::mat4 light_space_matrix = glm::mat4(1.0f);
		
		// drawing
		{
			glViewport(0, 0, depth_map_dimension, depth_map_dimension);

			float near_plane = 0.1f;
			float far_plane = 10000.0f;
			glm::mat4 light_projection_mat = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, near_plane, far_plane);

			glm::mat4 light_view_mat = glm::lookAt( 100.0f * directional_light_dir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


			glm::mat4 model_mat = glm::mat4(1.0f);
			model_mat = glm::scale(model_mat, glm::vec3(0.5f));
			light_space_matrix = light_projection_mat * light_view_mat;

			depth_shader.use();

			depth_shader.set_mat4("model_mat", model_mat);
			depth_shader.set_mat4("light_space_matrix", light_space_matrix);

			sponza.draw(depth_shader);
			
			model_mat = glm::mat4(1.0f);
			model_mat = glm::scale(model_mat, glm::vec3(10.0f));
			model_mat = glm::translate(model_mat, cube_position);

			cube.draw(depth_shader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		
		// Draw to rendertarget
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// drawing
		{
			glm::mat4 projection_mat = glm::perspective(glm::radians(g_camera.get_zoom()), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 10000.0f);
			glm::mat4 view_mat = g_camera.get_view_mat();
			
			glm::mat4 model_mat = glm::mat4(1.0f);
			model_mat = glm::scale(model_mat, glm::vec3(0.5f));

			shader.use();
			shader.set_vec3f("camera_position", g_camera.m_position);
			shader.set_vec3f("light_color", light_color);
			shader.set_vec3f("directional_light_dir", directional_light_dir);
			shader.set_mat4("light_space_matrix", light_space_matrix);
			shader.set_int("depth_map", depth_map);
			
			shader.set_mat4("model_mat", model_mat);
			shader.set_mat4("view_mat", view_mat);
			shader.set_mat4("projection_mat", projection_mat);

			sponza.draw(shader);
		}

		// drawing
		{
			glm::mat4 projection_mat = glm::perspective(glm::radians(g_camera.get_zoom()), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 10000.0f);
			glm::mat4 view_mat = g_camera.get_view_mat();

			glm::mat4 model_mat = glm::mat4(1.0f);
			model_mat = glm::scale(model_mat, glm::vec3(10.0f));
			model_mat = glm::translate(model_mat, cube_position);

			shader.use();
			shader.set_vec3f("camera_position", g_camera.m_position);
			shader.set_vec3f("light_color", light_color);
			shader.set_vec3f("directional_light_dir", directional_light_dir);
			shader.set_mat4("light_space_matrix", light_space_matrix);
			shader.set_int("depth_map", depth_map);


			
			shader.set_mat4("model_mat", model_mat);
			shader.set_mat4("view_mat", view_mat);
			shader.set_mat4("projection_mat", projection_mat);

			cube.draw(shader);
		}
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

	}

	glfwTerminate();
	return 0;
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
		g_last_x = xpos;
		g_last_y = ypos;

		g_first_mouse = false;
	}

	float xoffset = xpos - g_last_x;
	float yoffset = g_last_y - ypos;

	g_last_x = xpos;
	g_last_y = ypos;

	if (g_use_mouse)
	{
		g_camera.process_mouse(xoffset, yoffset);
	}
}

void process_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	g_camera.process_scroll(yoffset);
}
