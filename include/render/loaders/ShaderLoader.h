#pragma once

#include <GL/glew.h>

namespace render
{
	GLuint LoadShaders(const char* vertex_raw, GLint vertex_size, const char* fragment_raw, GLint fragment_size);
}