#pragma once

#include "OpenGL/OpenGL.h"
#include "OpenGL/OpenGL_Structs.h"

#include <string>

// STB Image header configuration
#define STBI_ONLY_PNG							// Specify that we only use the PNG format (reduces compilation time and binary size)
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"



class Texture
{
	private:
	
		// Texture ID & type
		GLuint TextureID;
		TextureType Type;
		PixelFormat cType;
		std::string Path;

	public:

		// Default constructor: Initialize diffuse texture with 0 id and no path; does not load the texture
		Texture() : 
			TextureID(0),
			Type(TextureType::diffuse),
			cType(PixelFormat::SRGBA),
			Path()
		{};
	
		// Full constructor; loads the texture (SENSIBLE?)
		Texture(
			const std::string& dir,
			TextureType TexType,
			PixelFormat ColType,
			bool generateMipMap = true
		) :
			TextureID(0),
			Type(TexType),
			cType(ColType),
			Path(dir)
		{
			load(generateMipMap);
		}
	
		// Destructor: request deletion from the OpenGL context if the id is non-zero (i.e. we actually loaded the texture)
		~Texture()
		{
			free();
		}
	
	
		// Accessors
		GLuint id() const					{ return TextureID; }
		TextureType texture_type() const	{ return Type; }
		PixelFormat colour_type() const 	{ return cType; }
		const std::string& path() const 	{ return Path; }

		// Free Texture: delete texture from OpenGL context and set the id to 0 (for later reuse)
		void free()
		{
			if (TextureID > 0) 
			{
				glDeleteTextures(1, &TextureID);
				TextureID = 0;
			}
		}

		// Load texture from file by specifying format specific parameters and whether OpenGL should automatically generate a mipmap
		void load(
			const std::string& dir,
			const TextureType TexType,
			const PixelFormat ColType,
			const bool generateMipMap = true
		)
		{
			// Setting the internal stuff
			Type = TexType;
			cType = ColType;
			Path = dir;

			// Do the loading
			load(generateMipMap);
		}
		
		// Load texture from the already specified
		void load(const bool generateMipMap)
		{	
			// Don't load the texture if it is already in
			if (TextureID != 0) 
			{
				return;
			}
		
			// Type management for STBI and OpenGL texture formatting
			int STBI_TYPE;
			GLint GLInternalFormat;
			GLenum GLFormat;
			
			// Choose the appropriate formats depending on the input
			switch(cType) 
			{
				case PixelFormat::SRGBA : 
					STBI_TYPE =	STBI_rgb_alpha;
					GLInternalFormat = GL_SRGB_ALPHA;
					GLFormat = GL_RGBA;
					break;
				case PixelFormat::RGBA :
					STBI_TYPE =	STBI_rgb_alpha;
					GLInternalFormat = GL_RGBA;
					GLFormat = GL_RGBA;
					break;
				case PixelFormat::SRGB :
					STBI_TYPE =	STBI_rgb;
					GLInternalFormat = GL_SRGB;
					GLFormat = GL_RGB;
					break;
				case PixelFormat::RGB :
					STBI_TYPE =	STBI_rgb;
					GLInternalFormat = GL_RGB;
					GLFormat = GL_RGB;
					break;
				case PixelFormat::R :
					STBI_TYPE =	STBI_grey;
					GLInternalFormat = GL_RED;
					GLFormat = GL_RED;
					break;
					
				default : 
					STBI_TYPE = STBI_default;
					GLInternalFormat = GL_SRGB_ALPHA;
					GLFormat = GL_RGBA;
					break;
			}
			
			// Picture Loading
			int width, height, components;
			unsigned char* image = stbi_load(Path.c_str(), &width, &height, &components, STBI_TYPE);

			// If the resulting image is empty, STBI ran into an error and thus we do nothing
			if (image == nullptr) 
			{
				return;
			}

			// Load the texture into OpenGL
			glGenTextures(1, &TextureID);
			glBindTexture(GL_TEXTURE_2D, TextureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GLInternalFormat, width, height, 0, GLFormat, GL_UNSIGNED_BYTE, image);
	
			// Generate mipmaps if desired
			if (generateMipMap) 
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			// Free memory and unbind the texture
			stbi_image_free(image);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
};