// render - A C++ OpenGL library
// Copyright (C) 2021 intrets

#pragma once

#include <wrangled_gl/wrangled_gl.h>

namespace render
{
	GLuint LoadShaders(const char* vertex_raw, GLint vertex_size, const char* fragment_raw, GLint fragment_size);
}