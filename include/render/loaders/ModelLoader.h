#pragma once

#include <GL/glew.h>
#include <wglm/glm.hpp>

#include <vector>
#include <string_view>

#include "render/BufferWrappers.h"

namespace render
{
	bool loadOBJ(
		std::string_view path,
		std::vector<glm::vec3>& out_vertices,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals
	);

	bwo::Model loadModel(std::string_view path);
}