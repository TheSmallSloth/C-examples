#version 450 core
in vec2 UV;

out vec4 FragmentColour;

uniform sampler2D HDR_Buffer;

uniform float exposure = 1.0;
uniform float contrast = 1.0;
uniform float inverse_gamma = 0.45;

uniform vec3 colour_filter = vec3(1.0);

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
	// 1.) Apply exposure: multiply with a non-negative scalar
	vec3 Colour = exposure * texture(HDR_Buffer, UV).rgb;

	// 2.) Apply colour filter: multiply with filter colour
	//Colour = vec3(0.72, 0.77, 0.45) * Colour;

	// 3.) Apply Contrast changes
	Colour = Contrast(Colour, contrast);

	// 4.) Apply Tone-Mapping
	//Colour = Hable(Colour, 7.0);
	Colour = ACES(Colour);

	// 5.) Apply Gamma correction
	Colour = pow(Colour, vec3(inverse_gamma));

	FragmentColour = vec4(Colour, 1.0);
}