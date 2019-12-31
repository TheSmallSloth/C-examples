#pragma once

#include "OpenGL/OpenGL_Structs.h"

namespace Renderer
{

struct Light 
{
	glm::vec3 Position;				// Position of the light source (point / spot lights)
	glm::vec3 Direction;			// Direction of the emitted light (directional / spot lights)
	glm::vec3 Colour;				// Colour ([0, 1]^3)
	float Intensity;				// Intensity
	float Cosine;					// Cosine of the half-angle of the spot light
	LightType Type;					// Type

	
	//////////////////
	// Constructors //
	//////////////////

	// Default constructor: Black light at the origin
	Light() :
		Position(0.0f),
		Direction(glm::uninitialize),
		Colour(0.0f),
		Intensity(0.0f),
		Cosine(0.0f),
		Type(LightType::point)
	{}

	// Full constructor
	template<typename PosType, typename DirType>
	Light(
		PosType Pos,
		DirType Dir,	
		const glm::vec3& Col,
		float I,
		float Cos,
		LightType T
	) :
		Position(Pos),
		Direction(Dir),
		Colour(Col),
		Intensity(I),
		Cosine(Cos),
		Type(T)
	{}
	

	// Default copy, assignment, move and destructor are fine
	Light(const Light&)				= default;
	Light(Light&&)					= default;
	Light& operator=(const Light&)	= default;
	Light& operator=(Light&&)		= default;
	~Light()						= default;


	/////////////////////////
	// Static constructors //
	/////////////////////////

	// Ambient light
	static Light Ambient(const glm::vec3& Colour, float Intensity)
	{
		return Light(
			glm::uninitialize,
			glm::uninitialize,
			glm::clamp(Colour, 0.0f, 1.0f),
			Intensity,
			0.0f,
			LightType::ambient
		);
	}


	// Directional light
	static Light Directional(const glm::vec3& Direction, const glm::vec3& Colour, float Intensity)
	{
		return Light(
			glm::uninitialize,
			glm::normalize(Direction),
			glm::clamp(Colour, 0.0f, 1.0f),
			Intensity,
			0.0f,
			LightType::directional
		);
	}

	// Point light
	static Light Point(const glm::vec3& Position, const glm::vec3& Colour, float Intensity)
	{
		return Light(
			Position,
			glm::uninitialize,
			glm::clamp(Colour, 0.0f, 1.0f),
			Intensity,
			0.0f,
			LightType::point
		);
	}

	// Spot light
	static Light Spot(const glm::vec3& Position, const glm::vec3& Direction, const glm::vec3& Colour, float Intensity, float AngleDegrees)
	{
		return Light(
			Position,
			glm::normalize(Direction),
			glm::clamp(Colour, 0.0f, 1.0f),
			Intensity,
			cos(glm::radians(AngleDegrees) / 2),
			LightType::spot
		);
	}
};

}

/*
    Candle 						glm::vec3(1.0f,	0.577f, 0.161f);
    Tungsten40W 				glm::vec3(1.0f,	0.773f, 0.561f);
    Tungsten100W 				glm::vec3(1.0f,	0.839f, 0.667f);
    Halogen 					glm::vec3(1.0f,	0.945f, 0.878f);
    CarbonArc 					glm::vec3(1.0f, 0.98f, 0.957f);
    HighNoonSun 				glm::vec3(1.0f,	1.0f, 0.984f);
    OvercastSky 				glm::vec3(0.788f, 0.886f, 1.0f);
    ClearBlueSky 				glm::vec3(0.25f, 0.612f, 1.0f);
    WarmFluorescent 			glm::vec3(1.0f,	0.967f, 0.898f);
    StandardFluorescent 		glm::vec3(0.957f, 1.0f, 0.98f);
    CoolWhiteFluorescent 		glm::vec3(0.831f, 0.922f, 1.0f);
    FullSpectrumFluorescent 	glm::vec3(1.0f,	0.967f, 0.949f);
    GrowLightFluorescent 		glm::vec3(1.0f,	0.937f, 0.969f);
    BlackLightFluorescent 		glm::vec3(0.655f, 0.0f, 1.0f);
    MercuryVapor 				glm::vec3(0.847f, 0.969f, 1.0f);
    SodiumVapor 				glm::vec3(1.0f,	0.82f, 0.698f);
    MetalHalide 				glm::vec3(0.949f, 0.988f, 1.0f);
    HighPressure Sodium 		glm::vec3(1.0f,	0.718f,	0.298f);
*/
