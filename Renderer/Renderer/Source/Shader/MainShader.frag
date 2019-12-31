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