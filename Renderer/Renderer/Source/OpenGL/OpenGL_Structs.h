#pragma once

#include "glext.h"



// Enum for texture types
enum class TextureType : GLuint 
{
	diffuse = 0,
	specular = 1,
	normal = 2
};


// Enum for light types
enum class LightType : GLint 
{
	ambient = 0,
	directional = 1,
	point = 2,
	spot = 3
};


// Enum for the different pixel formats
enum class PixelFormat
{
	SRGBA,
	RGBA,
	SRGB,
	RGB,
	R
};