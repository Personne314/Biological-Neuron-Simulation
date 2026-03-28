#include "camera.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>



/**
 * @brief Construct a camera looking toward a target.
 * @param target The target of the camera.
 */
Camera::Camera(glm::vec3 target) : 
	m_target(target),
	m_distance(500.0f),
	m_yaw(0.0f),
	m_pitch(0.0f),
	m_sensitivity(0.005f)
{
	update();
}

/**
 * @brief Rotate the camera of a certain amount around both axis.
 */
void Camera::rotate(float x_offset, float y_offset)
{
	m_yaw += x_offset * m_sensitivity;
	m_pitch -= y_offset * m_sensitivity;

	// Avoid gimbal lock.
	if (m_pitch > 1.55f) m_pitch = 1.55f;
	if (m_pitch < -1.55f) m_pitch = -1.55f;
	
	update();
}

/**
 * @brief Zoom toward the target.
 * @param offset The relate distance change toward the target.
 * The minimum zoom distance is 1.0f.
 */
void Camera::zoom(float offset)
{
	m_distance -= offset;
	if (m_distance < 1.0f) m_distance = 1.0f;
	update();
}

/**
 * @brief Calculate and return the view matrix.
 * @return The computed view matrix.
 */
glm::mat4 Camera::view() const
{
	return glm::lookAt(m_position, m_target, m_up);
}

/**
 * @brief Update the camera vectors.
 */
void Camera::update()
{
	m_position.x = m_target.x + m_distance * cos(m_pitch) * sin(m_yaw);
	m_position.y = m_target.y + m_distance * sin(m_pitch);
	m_position.z = m_target.z + m_distance * cos(m_pitch) * cos(m_yaw);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
}
