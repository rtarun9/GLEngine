#pragma once

#include <imgui.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>

struct GLFWwindow;

class UIManager
{
public:
	UIManager(GLFWwindow* windo);

	void present();

public:
};