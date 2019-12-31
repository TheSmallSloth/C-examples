#version 450 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;
layout (location = 3) in vec3 Bitangent;
layout (location = 4) in vec2 texCoords;

out vec3 VertexPosition;
out vec3 VertexNormal;
out vec2 UV;

vec4 Cache;

// Matrix Uniform struct
struct MATRIX { mat4 Model; mat4 ProjectionView; mat3 Normal; };
uniform MATRIX Matrix;

void main()
{
	// Transform UV such that it matches the picture coordinates <- TEMPORARY HACK
	UV = texCoords;

	// Transform vertices into world coordinates (cache intermediate result)
	Cache = Matrix.Model * vec4(Position, 1.0);
	VertexPosition = vec3(Cache);

	// Transform vertex normals into world coordinates
	VertexNormal = normalize(Matrix.Normal * Normal);

	// Apply view & projection to world vertices
	gl_Position = Matrix.ProjectionView * Cache;
}