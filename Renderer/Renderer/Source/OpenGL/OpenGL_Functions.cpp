
///////////////////////
// Helpful functions //
///////////////////////

// Compile shader from source code in form of char arrays for both vertex and fragment shader, returns 0 for failure
GLuint CompileShader(const char* VertexShaderSource, const char* FragmentShaderSource)
{
	GLuint Program, VertexShader, FragmentShader;
	GLint success = GL_FALSE;

	///////////////////
	// Vertex shader //
	///////////////////

	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
	glCompileShader(VertexShader);
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);

	// Vertex Shader Compilarion failure
	if (success == GL_FALSE) 
	{
		GLint logSize;
		glGetShaderiv(VertexShader, GL_INFO_LOG_LENGTH, &logSize);
		char* infoLog = new char[logSize + 1];
		glGetShaderInfoLog(VertexShader, logSize, NULL, infoLog);
		OutputDebugString(infoLog);
		OutputDebugString("Vertex Shader Compilation Error");
		delete[] infoLog;
		return 0U;
	}

	/////////////////////
	// Fragment shader //
	/////////////////////

	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderSource, NULL);
	glCompileShader(FragmentShader);
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &success);

	// Vertex Shader Compilarion failure
	if (success == GL_FALSE) 
	{
		GLint logSize;
		glGetShaderiv(FragmentShader, GL_INFO_LOG_LENGTH, &logSize);
		char* infoLog = new char[logSize + 1];
		glGetShaderInfoLog(FragmentShader, logSize, NULL, infoLog);
		OutputDebugString(infoLog);
		OutputDebugString("Fragment Shader Compilation Error");
		delete[] infoLog;
		return 0U;
	}

	Program = glCreateProgram();

	glAttachShader(Program, VertexShader);
	glAttachShader(Program, FragmentShader);

	glLinkProgram(Program);

	glDetachShader(Program, VertexShader);
	glDetachShader(Program, FragmentShader);

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	return Program;
}


// Generate a Depth Map FBO of given size
void GenerateDepthMapBuffer(
	GLuint& Out_FBO,
	GLuint& Out_Buffer,
	const unsigned int Width
)
{
	// Texture
	GLuint Buffer;
	glGenTextures(1, &Buffer);
	glBindTexture(GL_TEXTURE_2D, Buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, static_cast<GLsizei>(Width), static_cast<GLsizei>(Width), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Framebuffer
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Buffer, 0);
	glDrawBuffer(GL_NONE);						// Tell OpenGL explicitly that there will be no colour data
	glReadBuffer(GL_NONE);						// Dito

												// If the framebuffer is complete, we overwrite the output arguments
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		Out_FBO = FBO;
		Out_Buffer = Buffer;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// Generate an HDR Framebuffer of given width and height, optional argument specifies the samples for a multisampled buffer (default: no MSAA)
void GenerateHdrFrameBuffer(
	GLuint& Out_FBO,
	GLuint& Out_RBO,
	GLuint& Out_Buffer,
	const GLsizei Width,
	const GLsizei Height,
	const GLsizei Samples = 0
)
{
	// Variables to help differentiating between having MSAA or not
	const bool isMultisampled = static_cast<bool>(Samples);
	const GLenum TEXTURE_TYPE = isMultisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	// Colour attachment (RGBA16F, unclamped) as colour attachment for the FBO
	GLuint Buffer;
	glGenTextures(1, &Buffer);
	glBindTexture(TEXTURE_TYPE, Buffer);
	if (isMultisampled) 
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, GL_RGBA16F, Width, Height, GL_TRUE);
	}
	else 
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(TEXTURE_TYPE, 0);

	// Depth attachment for the FBO
	GLuint Depth;
	glGenRenderbuffers(1, &Depth);
	glBindRenderbuffer(GL_RENDERBUFFER, Depth);

	if (isMultisampled) 
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_DEPTH_COMPONENT, Width, Height);
	}
	else 
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);
	}

	// Create Framebuffer and attach attachments
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Depth);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TEXTURE_TYPE, Buffer, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// If the Framebuffer is complete, we override the input arguments
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) 
	{
		Out_FBO = FBO;
		Out_RBO = Depth;
		Out_Buffer = Buffer;
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// Generate intermediate HDR Framebuffer serving as in taking in the output of the multisampled Main Shader 
void GenerateIntermediateFrameBuffer(
	GLuint& Out_FBO,
	GLuint& Out_RBO,
	GLuint& Out_Buffer,
	const GLsizei Width,
	const GLsizei Height
)
{
	// Colour attachment (RGBA16F, unclamped) as colour attachment for the FBO
	GLuint Buffer;
	glGenTextures(1, &Buffer);
	glBindTexture(GL_TEXTURE_2D, Buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Depth attachment for the FBO
	GLuint Depth;
	glGenRenderbuffers(1, &Depth);
	glBindRenderbuffer(GL_RENDERBUFFER, Depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);

	// Create Framebuffer and attach attachments
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Depth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Buffer, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// If the Framebuffer is complete, we override the input arguments
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) 
	{
		Out_FBO = FBO;
		Out_RBO = Depth;
		Out_Buffer = Buffer;
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}