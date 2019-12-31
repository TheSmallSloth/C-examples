#version 450 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TextureCoordinates;

out vec2 UV;
void main() 
{
	UV = TextureCoordinates;
	gl_Position = vec4(Position, 1.0);
}