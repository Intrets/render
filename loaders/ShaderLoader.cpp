// render - A C++ OpenGL library
// Copyright (C) 2021 intrets

// Obtained and slightly modified from
// http://www.opengl-tutorial.org/download/
// https://github.com/opengl-tutorials/ogl/tree/master/common

#include "loaders/ShaderLoader.h"

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <misc/Logger.h>

namespace render
{
	GLuint LoadShaders(const char* vertex_raw, GLint vertex_size, const char* fragment_raw, GLint fragment_size) {
		logger->logInfo("Compiling Shaders\n");

		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		GLint Result = GL_FALSE;
		int32_t InfoLogLength = 0;

		// Compile Vertex Shader
		glShaderSource(VertexShaderID, 1, &vertex_raw, &vertex_size);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			logger->logError("Vertex shader error: {}\n", &VertexShaderErrorMessage[0]);
		}

		// Compile Fragment Shader
		glShaderSource(FragmentShaderID, 1, &fragment_raw, &fragment_size);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			logger->logError("Fragment shader error: {}\n", &FragmentShaderErrorMessage[0]);
		}

		// Link the program
		logger->logInfo("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			logger->logError("Program error: {}\n", &ProgramErrorMessage[0]);
		}

		glDetachShader(ProgramID, VertexShaderID);
		glDetachShader(ProgramID, FragmentShaderID);

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		logger->logInfo("Program ID: {}\n", ProgramID);

		return ProgramID;
	}
}