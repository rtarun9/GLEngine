#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

enum class Directions
{
	Forward,
	Backward,
	Left,
	Right
};

namespace defaults
{
	constexpr float YAW = -90.0f;
	constexpr float PITCH = 0.0f;
	constexpr float SPEED = 10.5f;
	constexpr float SENSITIVITY = 0.4f;
	constexpr float ZOOM = 45.0f;
}

class Camera
{
public:
	Camera();

	glm::mat4 get_view_mat();

	float get_zoom();

	void process_keyboard(float delta_time, Directions direction);
	void process_mouse(float xoffset, float yoffset);
	void process_scroll(float yoffset);

	void update_vectors();

public:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_world_up;

	float m_yaw;
	float m_pitch;
	float m_movement_speed;
	float m_mouse_sensitivity;
	float m_zoom;
};