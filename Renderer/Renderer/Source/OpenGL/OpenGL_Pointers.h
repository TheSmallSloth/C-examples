#pragma once

#include "glext.h"
#include <cstdio>



// Obtain function pointer to the OpenGL DLL from the function name 
void* GetAnyGLFuncAddress(const char* name)
{
	void *p = (void *)wglGetProcAddress(name);
	if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1))
	{
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(module, name);
	}

	return p;
}




PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLBINDVERTEXBUFFERSPROC glBindVertexBuffers;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;

PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;

PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM1UIPROC glUniform1ui;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLUNIFORM1IVPROC glUniform1iv;

PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;

PFNGLGENSAMPLERSPROC glGenSamplers;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
PFNGLBINDSAMPLERPROC glBindSampler;
PFNGLDELETESAMPLERSPROC glDeleteSamplers;

PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;

PFNGLDRAWBUFFERSPROC glDrawBuffers;

// Multisampling
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;


PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;


// Initialize the Function Pointers by getting them from the DLL
inline void InitializeFunctionPointers()
{
	// Buffers
	glGenBuffers = (PFNGLGENBUFFERSPROC)GetAnyGLFuncAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)GetAnyGLFuncAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)GetAnyGLFuncAddress("glBufferData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GetAnyGLFuncAddress("glDeleteBuffers");

	// Vertex (attribute) arrays
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)GetAnyGLFuncAddress("glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)GetAnyGLFuncAddress("glBindVertexArray");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetAnyGLFuncAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)GetAnyGLFuncAddress("glDisableVertexAttribArray");
	glBindVertexBuffers = (PFNGLBINDVERTEXBUFFERSPROC)GetAnyGLFuncAddress("glBindVertexBuffers");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GetAnyGLFuncAddress("glVertexAttribPointer");
	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)GetAnyGLFuncAddress("glDeleteVertexArrays");

	glActiveTexture = (PFNGLACTIVETEXTUREPROC)GetAnyGLFuncAddress("glActiveTexture");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)GetAnyGLFuncAddress("glGenerateMipmap");

	// Shader
	glCreateShader = (PFNGLCREATESHADERPROC)GetAnyGLFuncAddress("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)GetAnyGLFuncAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)GetAnyGLFuncAddress("glCompileShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)GetAnyGLFuncAddress("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)GetAnyGLFuncAddress("glAttachShader");
	glUseProgram = (PFNGLUSEPROGRAMPROC)GetAnyGLFuncAddress("glUseProgram");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)GetAnyGLFuncAddress("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GetAnyGLFuncAddress("glGetShaderInfoLog");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)GetAnyGLFuncAddress("glLinkProgram");
	glDetachShader = (PFNGLDETACHSHADERPROC)GetAnyGLFuncAddress("glDetachShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)GetAnyGLFuncAddress("glDeleteShader");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)GetAnyGLFuncAddress("glDeleteProgram");

	// Uniforms
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)GetAnyGLFuncAddress("glGetUniformLocation");
	glUniform1f = (PFNGLUNIFORM1FPROC)GetAnyGLFuncAddress("glUniform1f");
	glUniform1i = (PFNGLUNIFORM1IPROC)GetAnyGLFuncAddress("glUniform1i");
	glUniform1ui = (PFNGLUNIFORM1UIPROC)GetAnyGLFuncAddress("glUniform1ui");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)GetAnyGLFuncAddress("glUniform3fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)GetAnyGLFuncAddress("glUniformMatrix4fv");
	glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)GetAnyGLFuncAddress("glUniformMatrix3fv");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)GetAnyGLFuncAddress("glUniform1iv");

	// Framebuffer / Renderbuffer / ...
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)GetAnyGLFuncAddress("glBindFramebuffer");
	glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)GetAnyGLFuncAddress("glFramebufferTexture");
	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)GetAnyGLFuncAddress("glFramebufferTexture2D");
	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)GetAnyGLFuncAddress("glGenFramebuffers");
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)GetAnyGLFuncAddress("glCheckFramebufferStatus");
	glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)GetAnyGLFuncAddress("glBlitFramebuffer");
	glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)GetAnyGLFuncAddress("glGenRenderbuffers");
	glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)GetAnyGLFuncAddress("glBindRenderbuffer");
	glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)GetAnyGLFuncAddress("glRenderbufferStorage");
	glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)GetAnyGLFuncAddress("glFramebufferRenderbuffer");
	glDrawBuffers = (PFNGLDRAWBUFFERSPROC)GetAnyGLFuncAddress("glDrawBuffers");

	// Samplers
	glGenSamplers = (PFNGLGENSAMPLERSPROC)GetAnyGLFuncAddress("glGenSamplers");
	glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)GetAnyGLFuncAddress("glSamplerParameterf");
	glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)GetAnyGLFuncAddress("glSamplerParameteri");
	glBindSampler = (PFNGLBINDSAMPLERPROC)GetAnyGLFuncAddress("glBindSampler");
	glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)GetAnyGLFuncAddress("glDeleteSamplers");

	// Multi-sampling things
	glRenderbufferStorageMultisample =(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)GetAnyGLFuncAddress("glRenderbufferStorageMultisample");
	glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)GetAnyGLFuncAddress("glTexImage2DMultisample");
}


// Include additional helpful functions and structs
#include "OpenGL_Functions.cpp"