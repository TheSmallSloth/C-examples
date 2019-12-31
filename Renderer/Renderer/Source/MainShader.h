#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Internal
#include "Light.h"
#include "Texture.h"



////////////////////////
// Main Shader Source //
////////////////////////

// Vertex Shader
char* vertexShaderSource = R"(
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
)";

// Fragment shader
char* fragmentShaderSource = R"( 
	#version 450 core

	in vec3 VertexPosition;
	in vec3 VertexNormal;
	in vec2 UV;

	out vec4 Colour;

	const float depthMapRotX = 1.5 * 3.1416;
	const float AmbientIntensity = 0.10;

	vec3 N;
	float spec;

	uniform sampler2D DiffuseMap;
	uniform sampler2D SpecularMap;
	uniform sampler2D NormalMap;
	uniform sampler2D DepthMap;

	uniform vec3 cameraPosition;

	// Matrix Uniform struct
	struct MATRIX { mat4 Model; mat4 ProjectionView; mat3 Normal; };
	uniform MATRIX Matrix;

	// Light Uniform struct
	struct LIGHT { vec3 Position; vec3 Colour; vec3 Direction; float Cosine; int Type; };
	uniform LIGHT Light;

	uniform int Shininess;
	uniform bool hasNormalMap;
	uniform bool hasSpecularMap;
		
	void main()
	{
		// To be replaced
		vec3 depthMapVertPos = VertexPosition - Light.Position;
		depthMapVertPos.yz = vec2(depthMapVertPos.y*cos(depthMapRotX) + depthMapVertPos.z*sin(depthMapRotX), depthMapVertPos.z*cos(depthMapRotX) - depthMapVertPos.y*sin(depthMapRotX));
		depthMapVertPos.xy /= -depthMapVertPos.z;
		depthMapVertPos.x = (depthMapVertPos.x+1)/2;
		depthMapVertPos.y = (depthMapVertPos.y+1)/2;
		depthMapVertPos.z = (-depthMapVertPos.z/1000.0);

		// Use normal map if present, otherwise the vertex normal
		// Transform normal map information from colors ([0, 1]^3) to coordinates ([-1, 1]^3), 
		// flip y and z (necessary?!?!?) and transform into world space
		
		if(hasNormalMap)
		{
			N = 2 * texture(NormalMap, UV).rgb - 1.0;
			N.yz *= -1;
			N = normalize(Matrix.Normal * N);
		}
		else 
		{
			N = VertexNormal;
		}

		// Get the texture colour from the texture
		vec3 TextureColour = texture(DiffuseMap, UV).rgb;
		Colour.rgb = AmbientIntensity * TextureColour;
		Colour.a = 1.0;

		// To be replaced with a proper shadow computation and additional PCF on top
		if(texture(DepthMap, depthMapVertPos.xy).x > depthMapVertPos.z-0.0003)
		{

		// Lighting (Blinn-Phong with attenuation)
		vec3 L = (Light.Type == 1) ? -Light.Direction : normalize(Light.Position - VertexPosition);
		vec3 V = normalize(cameraPosition - VertexPosition);
		vec3 H = normalize(L + V);

		float diff = max(dot(L, N), 0.0);
		// Multiply spec with smoothing function of the diffuse bit to avoid hard edge, probably silly
		spec = pow(max(dot(N, H), 0.0), Shininess) * smoothstep(0.0, 1.0, diff) * (hasSpecularMap ? texture(SpecularMap, UV).r : 1.0);

		// Attenuation for Point and Spot lights
		float att = (Light.Type > 1) ? 1.0 / (1.0 + 0.35 * dot(Light.Position - VertexPosition, Light.Position - VertexPosition)) : 1.0;
		// Spot light: if the angle between the direction of the spot light and the 
		if(Light.Type == 3 && dot(-L, Light.Direction) < Light.Cosine)
		{
			att = 0.0;
		}
		diff *= att;
		spec *= att;

		Colour.rgb += diff * TextureColour + spec * Light.Colour;
		}
	}
)";


class MainShader
{
private:
	
	// Shader Program
	GLuint Program;

	// Uniform Locations
	struct MatrixUniforms { GLint Model, ProjectionView, Normal; };
	MatrixUniforms Matrix;

	struct LightUniforms { GLint Position, Direction, Colour, Cosine, Type; };
	LightUniforms Light;

	struct MaterialUniforms { GLuint Diffuse, Specular, Normal; GLint NormalBoolean, SpecularBoolean, Shininess; };
	MaterialUniforms Material;

	GLint CameraPosition;
	GLint DepthMap;

public:

	MainShader() = default;
	
	// Destructor: Deletes Program from OpenGL
	~MainShader()
	{
		glDeleteProgram(Program);
	}
	
	// Compile shaders and get uniform locations
	void instantiate()
	{
		// Compile shaders 
		Program = CompileShader(vertexShaderSource, fragmentShaderSource);
	
		// Get Main Shader Uniform locations
		Matrix.Model = glGetUniformLocation(Program, "Matrix.Model");
		Matrix.ProjectionView = glGetUniformLocation(Program, "Matrix.ProjectionView");
		Matrix.Normal = glGetUniformLocation(Program, "Matrix.Normal");

		Light.Position = glGetUniformLocation(Program, "Light.Position");
		Light.Direction = glGetUniformLocation(Program, "Light.Direction");
		Light.Colour = glGetUniformLocation(Program, "Light.Colour");
		Light.Cosine = glGetUniformLocation(Program, "Light.Cosine");
		Light.Type = glGetUniformLocation(Program, "Light.Type");

		CameraPosition = glGetUniformLocation(Program, "cameraPosition");
	
		Material.Diffuse = glGetUniformLocation(Program, "DiffuseMap");
		Material.Specular = glGetUniformLocation(Program, "SpecularMap");
		Material.SpecularBoolean = glGetUniformLocation(Program, "hasSpecularMap");
		Material.Normal = glGetUniformLocation(Program, "NormalMap");
		Material.NormalBoolean = glGetUniformLocation(Program, "hasNormalMap");
		Material.Shininess = glGetUniformLocation(Program, "Shininess");

		DepthMap = glGetUniformLocation(Program, "DepthMap");
	}

	/////////////
	// Setters //
	/////////////

	// Use the main shader program
	void use() const
	{
		glUseProgram(Program);
	}

	//////////////
	// Textures //
	//////////////

	// Setters for particular textures with set sampler and unit
	void setTexture(const Texture& tex) const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex.id());
	}

	void setSpecularMap(const Texture& spec) const
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, spec.id());
	}

	void setNormalMap(const Texture& normal) const
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normal.id());
	}

	void setDepthMap(const GLuint depthMap) const
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}

	
	// Assign the texture sampler uniforms to the appropriate texture units
	void assignTextureUnits() const
	{
		glUniform1i(Material.Diffuse, 0);
		glUniform1i(Material.Specular, 1);
		glUniform1i(Material.Normal, 2);
		glUniform1i(DepthMap, 3);
	}

	// Set Model and Normal matrix
	void setModelMatrices(const glm::mat4& ModelMatrix, const glm::mat3& NormalMatrix) const
	{
		glUniformMatrix4fv(Matrix.Model, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
		glUniformMatrix3fv(Matrix.Normal, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
	}

	// Set Projection * View Matrix
	void setProjectionViewMatrix(const glm::mat4& PVMatrix) const
	{
		glUniformMatrix4fv(Matrix.ProjectionView, 1, GL_FALSE, glm::value_ptr(PVMatrix));
	}

	// Light <- could be optimized to use less setX calls depending on the type of the light
	void setLight(const Renderer::Light& InLight) const
	{
		const glm::vec3 LightIntensity = InLight.Intensity * InLight.Colour;
		glUniform3fv(Light.Position, 1, glm::value_ptr(InLight.Position));
		glUniform3fv(Light.Direction, 1, glm::value_ptr(InLight.Direction));
		glUniform3fv(Light.Colour, 1, glm::value_ptr(LightIntensity));
		glUniform1f(Light.Cosine, InLight.Cosine);
		glUniform1i(Light.Type, static_cast<GLint>(InLight.Type));
	}

	// Set the camera position
	void setCameraPosition(const glm::vec3& CamPos) const
	{
		glUniform3fv(CameraPosition, 1, glm::value_ptr(CamPos));
	}

	// Set the specular exponent ("shininess")
	void setShininess(const int Shininess) const
	{
		glUniform1i(Material.Shininess, Shininess);
	}

	// Set the specular boolean (true iff there is a specular map present)
	void setSpecularMapBoolean(const bool hasSpecularMap) const
	{
		glUniform1i(Material.SpecularBoolean, static_cast<int>(hasSpecularMap));
	}

	// Set the normal map boolean (true iff there is a normal map present)
	void setNormalMapBoolean(const bool hasNormalMap) const
	{
		glUniform1i(Material.NormalBoolean, static_cast<int>(hasNormalMap));
	}
};