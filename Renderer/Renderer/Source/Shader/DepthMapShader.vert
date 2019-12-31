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