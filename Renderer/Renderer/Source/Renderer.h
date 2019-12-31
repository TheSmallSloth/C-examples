#pragma once

#include <vector>


#include "StringFunctions.cpp"
#include "Projection.h"
#include "Sampler.h"

struct ModelID
{
	int vertsPos;
	int vertsLength;
	std::vector<glm::vec3> uniqueSet;
	std::vector<std::vector<size_t>> edges;
};


namespace Renderer
{
	GLuint vao, pos, norm, tan, bitan, tex;
	GLuint QuadVAO, QuadVBO;

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> tangent;
	std::vector<GLfloat> bitangent;
	std::vector<GLfloat> texCoords;

	// Shaders
	DepthMapShader Shadow;
	MainShader Main;
	PostProcessingShader Post;
	
	// Framebuffer Objects
	GLuint depthMapFrameBuffer, depthMapTexture;
	GLuint IntermediateFBO, IntermediateRBO, IntermediateTexture;
	GLuint HDR_FBO, HDR_RBO, HDR_Texture;

	// Texture Sampler
	Sampler main_sampler;

	// Projection matrix
	glm::mat4 Projection;


	ModelID parseModel(LPCSTR dir)
	{
		HANDLE modelFile = CreateFile(dir, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		LARGE_INTEGER fileSize;
		GetFileSizeEx(modelFile, &fileSize);

		ModelID modelID;

		modelID.vertsPos = static_cast<int>(vertices.size() / 3);
		modelID.vertsLength = 0;

		char* data = new char[fileSize.QuadPart];

		ReadFile(modelFile, (void*)data, fileSize.QuadPart, NULL, NULL);
		CloseHandle(modelFile);

		size_t offset = 0;

		std::vector<GLfloat> tempVerts;
		std::vector<GLfloat> tempNorms;
		std::vector<GLfloat> tempTex;
		std::vector<size_t> index;

		while (!stringEqual(data + offset, "-mesh-positions-array", 21))
		{
			++offset;
		}
		while (data[offset] != '>')
		{
			++offset;
		}
		++offset;

		while (true)
		{
			tempVerts.push_back(parseFloat(data + offset));
			while (data[offset] != ' ' && data[offset] != '<')
			{
				++offset;
			}
			if (data[offset] == '<')
			{
				break;
			}
			while (data[offset] == ' ')
			{
				++offset;
			}
		}

		// Unique vertices
		for (size_t i = 0; i < tempVerts.size() / 3; ++i)
		{
			modelID.uniqueSet.emplace_back(tempVerts.at(3 * i), tempVerts.at(3 * i + 1), tempVerts.at(3 * i + 2));
		}

		while (!stringEqual(data + offset, "-mesh-normals-array", 19))
		{
			++offset;
		}
		while (data[offset] != '>')
		{
			++offset;
		}
		++offset;
		while (true)
		{
			tempNorms.push_back(parseFloat(data + offset));
			while (data[offset] != ' ' && data[offset] != '<')
			{
				++offset;
			}
			if (data[offset] == '<')
			{
				break;
			}
			while (data[offset] == ' ')
			{
				++offset;
			}
		}

		while (!stringEqual(data + offset, "-mesh-map-0-array", 17))
		{
			++offset;
		}
		while (data[offset] != '>')
		{
			++offset;
		}
		++offset;
		while (true)
		{
			tempTex.push_back(parseFloat(data + offset));
			while (data[offset] != ' ' && data[offset] != '<')
			{
				++offset;
			}
			if (data[offset] == '<')
			{
				break;
			}
			while (data[offset] == ' ')
			{
				++offset;
			}
		}

		while (!stringEqual(data + offset, "polylist", 8))
		{
			++offset;
		}
		while (!stringEqual(data + offset, "<p>", 3))
		{
			++offset;
		}
		offset += 3;
		while (true)
		{
			index.push_back(static_cast<size_t>(parseFloat(data + offset)));
			while ((data[offset] != ' ') && (data[offset] != '<'))
			{
				++offset;
			}
			if (data[offset] == '<')
			{
				break;
			}
			while (data[offset] == ' ')
			{
				++offset;
			}
		}

		delete[] data;

		// Write vertices/normals/... int the corresponding buffers
		for (size_t i = 0; 3 * i < index.size(); ++i)
		{
			vertices.push_back(tempVerts.at(3 * index.at(3 * i)));
			vertices.push_back(tempVerts.at(3 * index.at(3 * i)+1));
			vertices.push_back(tempVerts.at(3 * index.at(3 * i)+2));

			normals.push_back(tempNorms.at(3 * index.at(3 * i + 1)));
			normals.push_back(tempNorms.at(3 * index.at(3 * i + 1)+1));
			normals.push_back(tempNorms.at(3 * index.at(3 * i + 1)+2));

			texCoords.push_back(tempTex.at(2 * index.at(3 * i + 2)));
			texCoords.push_back(tempTex.at(2 * index.at(3 * i + 2)+1));

			++modelID.vertsLength;
		}
		
		// Compute the edges (as indices of the neighbouring vertices at the vertex index)
		modelID.edges.resize(modelID.uniqueSet.size());

		for (size_t i = 0; i < index.size() / 9; ++i)
		{
			bool add = true;
			for (size_t j = 0; j < modelID.edges.at(index.at(9 * i)).size(); j++)
			{
				if (modelID.edges.at(index.at(9 * i)).at(j) == index.at(9 * i + 3))
				{
					add = false;
					break;
				}
			}
			if (add)
			{
				modelID.edges.at(index.at(9 * i)).push_back(index.at(9 * i + 3));
			}
			add = true;

			for (size_t j = 0; j < modelID.edges.at(index.at(9 * i)).size(); ++j)
			{
				if (modelID.edges.at(index.at(9 * i)).at(j) == index.at(9 * i + 6))
				{
					add = false;
					break;
				}
			}
			if (add)
			{
				modelID.edges.at(index.at(9 * i)).push_back(index.at(9 * i + 6));
			}
			add = true;

			for (size_t j = 0; j < modelID.edges.at(index.at(9 * i + 3)).size(); j++)
			{
				if (modelID.edges.at(index.at(9 * i + 3)).at(j) == index.at(9 * i))
				{
					add = false;
					break;
				}
			}
			if (add)
			{
				modelID.edges.at(index.at(9 * i + 3)).push_back(index.at(9 * i));
			}
			add = true;

			for (size_t j = 0; j < modelID.edges.at(index.at(9 * i + 3)).size(); j++)
			{
				if (modelID.edges.at(index.at(9 * i + 3)).at(j) == index.at(9 * i + 6))
				{
					add = false;
					break;
				}
			}
			if (add)
			{
				modelID.edges.at(index.at(9 * i + 3)).push_back(index.at(9 * i + 6));
			}
			add = true;

			for (size_t j = 0; j < modelID.edges.at(index.at(9 * i + 6)).size(); j++)
			{
				if (modelID.edges.at(index.at(9 * i + 6)).at(j) == index.at(9 * i))
				{
					add = false;
					break;
				}
			}
			if (add)
			{
				modelID.edges.at(index.at(9 * i + 6)).push_back(index.at(9 * i));
			}
			add = true;

			for (size_t j = 0; j < modelID.edges.at(index.at(9 * i + 6)).size(); j++)
			{
				if (modelID.edges.at(index.at(9 * i + 6)).at(j) == index.at(9 * i + 3))
				{
					add = false;
					break;
				}
			}
			if (add)
			{
				modelID.edges.at(index.at(9 * i + 6)).push_back(index.at(9 * i + 3));
			}
		}
	
		return modelID;
	}

	// Fill the vertex/normal/... buffers with data from the models
	void fillBuffers()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Vertex buffer
		glGenBuffers(1, &pos);
		glBindBuffer(GL_ARRAY_BUFFER, pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		// Normal buffer
		glGenBuffers(1, &norm);
		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		// Tangent buffer
		glGenBuffers(1, &tan);
		glBindBuffer(GL_ARRAY_BUFFER, tan);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tangent.size(), tangent.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);

		// Bitangent buffer
		glGenBuffers(1, &bitan);
		glBindBuffer(GL_ARRAY_BUFFER, bitan);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bitangent.size(), bitangent.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(3);

		// Texture coordinate buffer
		glGenBuffers(1, &tex);
		glBindBuffer(GL_ARRAY_BUFFER, tex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);

		//////////////
		// HDR quad //
		//////////////

		// Quad Vertex Positions and Texture Coordinates bundled into 5-dim vectors (first 3 components positions, last 2 UV)
		constexpr GLfloat Quad[] = {
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		// Quad VAO
		glGenVertexArrays(1, &QuadVAO);
		glGenBuffers(1, &QuadVBO);
		glBindVertexArray(QuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Quad), &Quad, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glBindVertexArray(0);


		// Generate various Frame Buffers
		GenerateDepthMapBuffer(depthMapFrameBuffer, depthMapTexture, 800);									// A single Depth Map Buffer of given resolution
		GenerateHdrFrameBuffer(HDR_FBO, HDR_RBO, HDR_Texture, 1280, 720, 4);								// HDR Framebuffer: (multisampled) buffer 
		GenerateIntermediateFrameBuffer(IntermediateFBO, IntermediateRBO, IntermediateTexture, 1280, 720);	// Intermediate HDR framebuffer solely for drawing HDR texture to Quad

		Projection = Renderer::InfinitePerspective(60.0f, 1280, 720, 0.1f);

		// Set the clear colour to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// Enable depth testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearDepth(1.0f);

		// Enable face culling
		glEnable(GL_CULL_FACE);			
	}


	// Generate a texture sampler with anisotropic filtering and bind it to the texture units 0-2 (texture / normal / specular)
	void generateSamplers()
	{
		// Initialize sampler 
		main_sampler = Sampler(16);

		// Bind sampler to texture units
		main_sampler.bind_to_texture_unit(0);	// Bind sampler to 	TEXTURE0
		main_sampler.bind_to_texture_unit(1);	//  "	 "		"  	TEXTURE1
		main_sampler.bind_to_texture_unit(2);	//  "	 "		"  	TEXTURE2
	}

	void initialiseShaders()
	{
		Shadow.instantiate();
		Main.instantiate();
		Post.instantiate();
	}


	void cleanUp()
	{
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);
		glDisableVertexAttribArray(6);
		glDisableVertexAttribArray(7);

		glDeleteVertexArrays(1, &vao);
		glDeleteVertexArrays(1, &QuadVAO);

		glDeleteBuffers(1, &pos);
		glDeleteBuffers(1, &norm);
		glDeleteBuffers(1, &tan);
		glDeleteBuffers(1, &bitan);
		glDeleteBuffers(1, &tex);
	}

	void clearBuffers()
	{
		// Clear does not resize the vectors (!)
		vertices.clear();
		normals.clear();
		tangent.clear();
		bitangent.clear();
		texCoords.clear();

		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);

		glDeleteBuffers(1, &pos);
		glDeleteBuffers(1, &norm);
		glDeleteBuffers(1, &tan);
		glDeleteBuffers(1, &bitan);
		glDeleteBuffers(1, &tex);

		glDeleteVertexArrays(1, &vao);
		glDeleteVertexArrays(1, &QuadVAO);
	}

	void PrepareDepthMapPass()
	{
		glBindVertexArray(Renderer::vao);

		glViewport(0, 0, 800, 800);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFrameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		Shadow.use();
	}

	void PrepareDrawPass(const glm::mat4& View = glm::mat4(1.0f))
	{
		glViewport(0, 0, 1280, 720);
		glBindFramebuffer(GL_FRAMEBUFFER, HDR_FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Main.use();
		Main.assignTextureUnits();

		Main.setDepthMap(depthMapTexture);
		Main.setProjectionViewMatrix(Projection * View);
	}


	void PostProcessPass(float Exposure = 1.0f, float Gamma = 2.2f, float Contrast = 1.0f, const glm::vec3& ColourFilter = glm::vec3(1.0f))
	{
		// Put the contents of the HDR texture into the intermediate framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, HDR_FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, IntermediateFBO);
		glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// Switch to the default framebuffer and draw to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable the post-processing shader and set the scene parameters
		Post.use();
		Post.setHdrBuffer(IntermediateTexture);

		Post.setExposureAndFilter(Exposure, ColourFilter);
		Post.setGamma(Gamma);
		Post.setContrast(Contrast);

		// Draw the final texture to the quad (requires binding and unbinding the QuadVAO)
		glBindVertexArray(QuadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
}