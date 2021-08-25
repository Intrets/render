#pragma once

#include <juce_opengl/opengl/juce_gl.h>

using namespace juce::gl;

namespace render
{
	GLuint LoadShaders(const char* vertex_raw, const char* fragment_raw);
}