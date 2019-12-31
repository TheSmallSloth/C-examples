#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Namespace
namespace Renderer {

// Orthographic projection matrix
glm::mat4 Orthographic(
	const float left, 
	const float right, 
	const float bottom, 
	const float top, 
	const float zNear, 
	const float zFar
)
{
	return glm::ortho(left, right, bottom, top, zNear, zFar);
}


// Perspective projection matrix specified by the vertical FOV (given in degrees), the aspect ratio and the near and far z limits of the frustum 
glm::mat4 Perspective(
	const float FOVyDegrees, 
	const unsigned int Width, 
	const unsigned int Heigth, 
	const float zNear, 
	const float zFar
)
{
	return glm::perspective(glm::radians(FOVyDegrees), static_cast<float>(Width) / static_cast<float>(Heigth), zNear, zFar);
}

// Infinite Perspective projection matrix specified by vertical FOV in degrees, the aspect ratio and the near limit for z
glm::mat4 InfinitePerspective(
	float FOVyDegrees,
	unsigned int Width, 
	unsigned int Heigth, 
	float zNear
)
{
	return glm::infinitePerspective(glm::radians(FOVyDegrees), static_cast<float>(Width) / static_cast<float>(Heigth), zNear);
}

} // /namespace