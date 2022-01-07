#include "../include/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

Camera::Camera()
	: m_yaw(defaults::YAW), m_pitch(defaults::PITCH), m_mouse_sensitivity(defaults::SENSITIVITY),
	  m_movement_speed(defaults::SPEED), m_zoom(defaults::ZOOM)
{
	m_position = glm::vec3(glm::vec3(0.0f, 0.0f, 3.0f));
	m_world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_right = glm::vec3(1.0f, 0.0f, 0.0f);
	m_up = m_world_up;
	m_front = glm::vec3(0.0f, 0.0f, -1.0f);

	update_vectors();
}

glm::mat4 Camera::get_view_mat()
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

float Camera::get_zoom()
{
	return m_zoom;
}

void Camera::process_keyboard(float delta_time, Directions direction)
{
	float speed = m_movement_speed * delta_time;
	if (direction == Directions::Forward)
	{
		m_position += m_front * speed;
	}
	else if (direction == Directions::Backward)
	{
		m_position -= m_front * speed;
	}

	if (direction == Directions::Right)
	{
		m_position += m_right * speed;
	}
	else if (direction == Directions::Left)
	{
		m_position -= m_right * speed;
	}

	update_vectors();
}

void Camera::process_mouse(float xoffset, float yoffset)
{
	xoffset *= m_mouse_sensitivity;
	yoffset *= m_mouse_sensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

	update_vectors();
}

void Camera::process_scroll(float yoffset)
{
	m_zoom -= yoffset;
	m_zoom = std::clamp(m_zoom, 1.0f, 45.0f);
}

void Camera::update_vectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

	m_front = glm::normalize(front);
	m_right = glm::normalize(glm::cross(m_front, m_world_up));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}