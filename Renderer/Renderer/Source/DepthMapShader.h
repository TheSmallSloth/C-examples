#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Internal
#include "Light.h"

/////////////////////////////
// Depth Map Shader Source //
/////////////////////////////

// Vertex
char* vertexShaderSourceDepthMap = R"(
	#version 450 core

	layout (location = 0) in vec3 Position;

	const float depthMapRotX = 1.5 * 3.1416;

	uniform vec3 cameraPosition;
	uniform mat4 Model;

	void main() 
	{
		// To be replaced with matrix multiplication
		vec4 newPos = Model * vec4(Position, 1.0) - vec4(cameraPosition, 0.0);
		newPos.yz = vec2(newPos.y*cos(depthMapRotX) + newPos.z*sin(depthMapRotX), newPos.z*cos(depthMapRotX) - newPos.y*sin(depthMapRotX));
		gl_Position.xy = newPos.xy;
		gl_Position.z = (((-newPos.z)/500.0)-1.0)*-newPos.z;
		gl_Position.w = -newPos.z;
	}
)";

// Fragment
char* fragmentShaderSourceDepthMap = R"(
	#version 450 core
	
	void main()
	{
		gl_FragDepth = gl_FragCoord.z;
	}
)";



class DepthMapShader
{
private:
	
	// Shader Program
	GLuint Program;

	// Uniform locations
	GLint cameraPosition;
	GLint modelMatrix;

public:

	DepthMapShader() = default;

	// Destructor: Deletes Program from OpenGL
	~DepthMapShader()
	{
		glDeleteProgram(Program);
	}
	
	// Compile shaders and get uniform locations
	void instantiate()
	{
		// Compile shaders 
		Program = CompileShader(vertexShaderSourceDepthMap, fragmentShaderSourceDepthMap);

		// Get Depth Map Shader uniform locations
		cameraPosition = glGetUniformLocation(Program, "cameraPosition");
		modelMatrix = glGetUniformLocation(Program, "Model");
	}

	/////////////
	// Setters //
	/////////////

	// Use the depth map shader program
	void use() const
	{
		glUseProgram(Program);
	}

	// TO BE REPLACED
	void setLightPosition(const glm::vec3& CameraPosition) const
	{
		glUniform3fv(cameraPosition, 1, glm::value_ptr(CameraPosition));
	}

	// Set the Model matrix in the depth map shader
	void setModelMatrix(const glm::mat4& ModelMatrix) const
	{
		glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
	}
};