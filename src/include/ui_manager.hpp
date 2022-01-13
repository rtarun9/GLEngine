#pragma once

#include <imgui.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

struct GLFWwindow;

class UIManager
{
public:
	UIManager(GLFWwindow* windo);

	// temporarily added here to make adding parameters easy for me.
	void render(ImVec4& clear_color, float& camera_speed)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Scene Control");
		ImGui::ColorEdit3("clear_color", (float*)&clear_color);
		ImGui::SliderFloat("camera_speed", &camera_speed, 0.0f, 1000.0f);

		ImGui::End();
		ImGui::Render();
	}

	void present();

public:
};