#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Textures; TO BE ELIMINATED EVENTUALLY
Texture redTexture, blueTexture, greenTexture, yellowTexture;
Texture CubeDiffuse, CubeSpecular, CubeNormal;

// Functions loading the textures; TO BE ELIMINATED EVENTUALLY
void loadTextures()
{
	redTexture.load("Resources/Red.png", TextureType::diffuse, PixelFormat::RGBA, false);
	blueTexture.load("Resources/Blue.png", TextureType::diffuse, PixelFormat::RGBA, false);
	greenTexture.load("Resources/Green.png", TextureType::diffuse, PixelFormat::RGBA, false);
	yellowTexture.load("Resources/Yellow.png", TextureType::diffuse, PixelFormat::RGBA, false);
}

void loadCubeStuff()
{
	CubeDiffuse.load("Resources/Cube/CubeDiffuse.png", TextureType::diffuse, PixelFormat::SRGBA);
	CubeSpecular.load("Resources/Cube/CubeSpecularGrey.png", TextureType::specular, PixelFormat::R);
	CubeNormal.load("Resources/Cube/CubeNormal.png", TextureType::normal, PixelFormat::R);
}

// GameObjects

class Sphere
{

public:

	bool isRendered;
	int specularity;
	double lifeTime;

	glm::vec3 Scale;

	glm::vec3 Position;
	std::vector<fizz::Hitshape3d> HitBox;
	size_t PhysicsIndex;

	glm::mat4 ModelMatrix;
	glm::mat3 NormalMatrix;

	Texture texture;

	static ModelID modelID;

	Sphere() = delete;
	Sphere(const int spec, const glm::vec3& InPosition, const float scale = 1.0f) :
		isRendered(true),
		specularity(spec),
		lifeTime(0),
		Scale(scale),
		Position(InPosition),
		HitBox({ fizz::Hitshape3d::Ball(scale) }),
		PhysicsIndex(0),
		ModelMatrix(glm::translate(glm::scale(Scale), Position)),
		NormalMatrix(1.0f)
	{
		static int texNum = 0;
		switch (texNum) 
		{
			case 0 : texture = redTexture; 
				break;
			case 1 : texture = blueTexture;
				break;
			case 2 : texture = greenTexture;
				break;
			case 3 : texture = yellowTexture; 
				break;
		}
		texNum = (texNum + 1) % 4;
	}

	void update(const double deltaT)
	{
		lifeTime += deltaT;
	}

	// Update Model and Normal matrices from a given transformation
	void updateMatrices(const fizz::Trafo3d& A)
	{
		const glm::mat3 Rotation = glm::mat3_cast(A.rotation());
		ModelMatrix = glm::scale(glm::translate(static_cast<glm::mat4>(Rotation), A.translation()), A.scale() * Scale);
		NormalMatrix[0] = (1.0f / Scale[0]) * Rotation[0];
		NormalMatrix[1] = (1.0f / Scale[1]) * Rotation[1];
		NormalMatrix[2] = (1.0f / Scale[2]) * Rotation[2];
	}
	

	// Draw
	void draw(MainShader& shader) const
	{
		if (isRendered)
		{
			shader.setModelMatrices(ModelMatrix, NormalMatrix);

			shader.setShininess(specularity);
			shader.setTexture(texture);
			shader.setNormalMapBoolean(false);
			shader.setSpecularMapBoolean(false);

			glDrawArrays(GL_TRIANGLES, modelID.vertsPos, modelID.vertsLength);
			//glDrawArrays(GL_LINE_LOOP, modelID.vertsPos, modelID.vertsLength);		
		}
	}


	// Draw shadows
	void drawShadow(DepthMapShader& shader) const
	{
		if (isRendered)
		{
			shader.setModelMatrix(ModelMatrix);
			glDrawArrays(GL_TRIANGLES, modelID.vertsPos, modelID.vertsLength);
			//glDrawArrays(GL_LINE_LOOP, modelID.vertsPos, modelID.vertsLength);		
		}
	}

	// Set visibility boolean
	void setVisible(const bool visible)
	{
		isRendered = visible;
	}
};

ModelID Sphere::modelID = Renderer::parseModel("Resources/sphere.dae");

class Cube
{
public:
	bool isRendered;
	int specularity;
	double lifeTime;

	glm::vec3 Scale;
	
	glm::vec3 Position;
	std::vector<fizz::Hitshape3d> HitBox;
	size_t PhysicsIndex;

	glm::mat4 ModelMatrix;
	glm::mat3 NormalMatrix;

	Texture texture, specular, normal;

	static ModelID modelID;

	Cube() = delete;
	Cube(const int spec, const glm::vec3& InPosition, const float scale = 1.0f) :
		isRendered(true),
		specularity(spec),
		lifeTime(0),
		Scale(scale),
		Position(InPosition),
		HitBox({ fizz::Hitshape3d::Cube(2 * scale) }),
		PhysicsIndex(0),
		ModelMatrix(glm::translate(glm::scale(Scale), Position)),
		NormalMatrix(1.0f)
	{
		texture = CubeDiffuse; 
		specular = CubeSpecular;
		normal = CubeNormal;
	}

	void update(const double deltaT)
	{
		lifeTime += deltaT;
	}

	// Update Model and Normal matrices from a given transformation
	void updateMatrices(const fizz::Trafo3d& A)
	{
		glm::mat3 Rotation = glm::mat3_cast(A.rotation());
		ModelMatrix = glm::scale(glm::translate(static_cast<glm::mat4>(Rotation), A.translation()), A.scale() * Scale);
		NormalMatrix[0] = (1.0f / Scale[0]) * Rotation[0];
		NormalMatrix[1] = (1.0f / Scale[1]) * Rotation[1];
		NormalMatrix[2] = (1.0f / Scale[2]) * Rotation[2];
	}
	
	// Draw
	void draw(MainShader& shader) const
	{
		if (isRendered)
		{
			shader.setModelMatrices(ModelMatrix, NormalMatrix);

			shader.setShininess(specularity);
			shader.setTexture(texture);
			shader.setSpecularMap(specular);
			shader.setSpecularMapBoolean(true);
			shader.setNormalMap(normal);
			shader.setNormalMapBoolean(false);

			glDrawArrays(GL_TRIANGLES, modelID.vertsPos, modelID.vertsLength);
			//glDrawArrays(GL_LINE_LOOP, modelID.vertsPos, modelID.vertsLength);
		}
	}

	// Draw shadows
	void drawShadow(DepthMapShader& shader) const
	{
		if (isRendered)
		{
			shader.setModelMatrix(ModelMatrix);

			glDrawArrays(GL_TRIANGLES, modelID.vertsPos, modelID.vertsLength);
			//glDrawArrays(GL_LINE_LOOP, modelID.vertsPos, modelID.vertsLength);
		}
	}

	// Set the visibility boolean
	void setVisible(const bool visible)
	{
		isRendered = visible;
	}
};

ModelID Cube::modelID = Renderer::parseModel("Resources/cube.dae");