#pragma once

#include <glm/glm.hpp>



/**
 * @class Camera
 * @brief Simple 3D camera used to rotate arround a point. 
 */
class Camera
{
public:

	Camera(glm::vec3 target = glm::vec3(0.0f));

	void rotate(float x_offset, float y_offset);
	void zoom(float offset);
	
	glm::mat4 view() const;
	
	glm::vec3 position() const { return m_position; }

private:

	void update();

	glm::vec3 m_position; // Camera position in the world space.
	glm::vec3 m_target;   // Target looked by the camera.
	glm::vec3 m_up;       // Up vector.

	float m_distance;    // Distance between the camera and the center.
	float m_yaw;         // Yaw angle.
	float m_pitch;       // Pitch angle.
	float m_sensitivity; // Camera sensitivity in radians.

};
