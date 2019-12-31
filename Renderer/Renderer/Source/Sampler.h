#pragma once

#include "OpenGL/OpenGL.h"

class Sampler
{
private:
	
	// Sampler ID
	GLuint sampler;

public:

	// Accessing the GLuint
	GLuint id() const { return sampler; }

	// Free Texture: delete texture from the OpenGL context and set the id to 0
	void free()
	{
		if (sampler != 0) {
			glDeleteSamplers(1, &sampler);
			sampler = 0;
		}
	}	

	// Set integer sampler parameter
	void set_parameter_i(GLenum pname, GLint param) const
	{
		glSamplerParameteri(sampler, pname, param);
	}

	// Set float sampler parameter
	void set_parameter_f(GLenum pname, GLfloat param) const
	{
		glSamplerParameterf(sampler, pname, param);
	}

	// Bind sampler to given texture unit
	void bind_to_texture_unit(GLuint unit) const
	{
		glBindSampler(unit, sampler);
	}
	

	//////////////////
	// Constructors //
	//////////////////
	
	// Default constructor: 0
	Sampler() : sampler(0) {};
	
	// Anisotropic Sampler
	Sampler(const unsigned int Anisotropy) :
		sampler(0)
	{
		glGenSamplers(1, &sampler);
//		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);						// Magnification: linear filtering
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		// Minification: linear filtering with mipmaps

		if ((Anisotropy > 0) && (Anisotropy <= 16)) {
			glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLfloat>(Anisotropy));
		}
	}
	
	// "Full" constructor
	Sampler(
			GLenum pname0, GLint param0,
			GLenum pname1, GLint param1,
			unsigned int Anisotropy = 0
	) :
		sampler(0)
	{
		glGenSamplers(1, &sampler);
		glSamplerParameteri(sampler, pname0, param0);
		glSamplerParameteri(sampler, pname1, param1);

		if ((Anisotropy > 0) && (Anisotropy <= 16)) 
		{
			glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLfloat>(Anisotropy));
		}
	}
	
	// Destructor: Delete texture
	~Sampler()
	{
		if (sampler != 0) 
		{
			glDeleteSamplers(1, &sampler);
		}
	}
};