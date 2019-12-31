#pragma once

// STL
#include <cmath>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Internal
#include "Light.h"

///////////////////////
// HDR Shader Source //
///////////////////////

// Vertex
char* vertexShaderSourceHDR = R"(
	#version 450 core
	
	layout (location = 0) in vec3 Position;
	layout (location = 1) in vec2 TextureCoordinates;

	out vec2 UV;

	void main() 
	{
		UV = TextureCoordinates;
		gl_Position = vec4(Position, 1.0);
	}
)";

// Fragment
char* fragmentShaderSourceHDR = R"(
	#version 450 core

	in vec2 UV;

	out vec4 FragmentColour;

	uniform sampler2D HDR_Buffer;

	uniform float contrast = 1.0;
	uniform float inverse_gamma = 0.45;
	uniform vec3 exposure_times_filter = vec3(1.0);

	// Approximate ACES tone mapping
	vec3 ACES(const vec3 x)
	{
		vec3 nominator = (0.9036 * x + 0.018) * x;
		vec3 denominator = (0.8748 * x + 0.354) * x + 0.14;
		return (nominator / denominator);
	}

	// Hable tone mapping
	vec3 Hable(const vec3 LinearColour, const float White)
	{
		vec3 Q = (3.0 * LinearColour + 10.0) * LinearColour;
		float W_inverse = 1.0  / (((3.0 * White + 1.0) * White + 0.08) / ((3.0 * White + 10.0) * White + 1.2) - 1.0 / 15.0);
		return  W_inverse * ( (Q - 9.0 * LinearColour + 0.08) / (Q + 1.2) - vec3(1.0 / 15.0) );
	}


	// Contrast map
	vec3 Contrast(const vec3 Colour, const float Contrast)
 	{	
		// Maybe the log-space linear interpolation is faster than the direct power computation?
		//float LogMidpoint = log2(0.18);
 		//vec3 LogColour = Contrast * (log2(Colour + 0.001) - LogMidpoint) + LogMidpoint;	
 		//return max(exp2(LogColour) - 0.001, 0.0);
		return 0.18 * pow(Colour / 0.18, vec3(Contrast));
 	}

	// LiftGammaGain() 
	// {
	// }

	void main()
	{
		// 1.) Apply exposure & colour filter: multiply with a non-negative scalar
		vec3 Colour = exposure_times_filter* texture(HDR_Buffer, UV).rgb;

		// 3.) Apply Contrast changes
		Colour = Contrast(Colour, contrast);

		// 4.) Apply Tone-Mapping
		//Colour = Hable(Colour, 7.0);
		Colour = ACES(Colour);

		// 5.) Apply Gamma correction
		Colour = pow(Colour, vec3(inverse_gamma));

		FragmentColour = vec4(Colour, 1.0);
	}
)";


class PostProcessingShader
{
	private:
	
		// Shader Program
		GLuint Program;
		
		// Input texture sampler
		GLuint Sampler;

		// Uniform locations for the post-processing parameters
		GLint ExposureTimesFilter;
		GLint InverseGamma;
		GLint Contrast;

	public:

		PostProcessingShader() = default;

		// Destructor: Deletes Program from OpenGL
		~PostProcessingShader()
		{
			glDeleteProgram(Program);
		}
	
		// Compile shaders and get uniform locations
		void instantiate()
		{
			// Compile shaders 
			Program = CompileShader(vertexShaderSourceHDR, fragmentShaderSourceHDR);

			// Get HDR Shader uniform locations
			Sampler = glGetUniformLocation(Program, "HDR_Buffer");					// Input texture that will be mapped onto the screen
	
			ExposureTimesFilter = glGetUniformLocation(Program, "exposure_times_filter");	// Exposure
			InverseGamma = glGetUniformLocation( Program, "inverse_gamma" );		// 1/Gamma
			Contrast = glGetUniformLocation( Program, "contrast" );					// Contrast
		}

		/////////////
		// Setters //
		/////////////

		// Use the depth map shader program
		void use() const
		{
			glUseProgram(Program);
		}

		// Set the exposure as f-stops: multiply intensities with 2^f
		void setExposureAndFilter(const float f, const glm::vec3& filter = glm::vec3(1.0f)) const
		{
			glm::vec3 ExpTimesFilter = std::exp2(f) * filter;
			glUniform3fv(ExposureTimesFilter, 1, glm::value_ptr(ExpTimesFilter));
		}


		// Set the exposure as f-stops: multiply intensities with 2^F, F being the F-stop
		void setGamma(const float Gamma) const
		{
			glUniform1f(InverseGamma, 1.0f / Gamma);
		}

		// Set the contrast
		void setContrast(const float C) const
		{
			glUniform1f(Contrast, C);
		}


		// Bind the sampler to the input texture
		void setHdrBuffer(const GLuint Buffer) const
		{
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, Buffer);
			glUniform1i(Sampler, 4);
		}
};