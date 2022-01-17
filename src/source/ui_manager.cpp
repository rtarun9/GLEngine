#include "../include/ui_manager.hpp"


UIManager::UIManager(GLFWwindow* window)
{
	// Set up imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void UIManager::present()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}