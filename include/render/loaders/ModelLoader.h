// render - A C++ OpenGL library
// Copyright (C) 2022 intrets

#pragma once

#include <wglm/vec2.hpp>
#include <wglm/vec3.hpp>

#include <vector>
#include <string_view>

#include "render/BufferWrappers.h"
#include "render/RendererTemplate.h"

#include <memory>
#include <resource_data/ResourceData.h>

namespace render
{
	bool loadOBJ(
		std::unique_ptr<Buffer> const& buffer,
		std::vector<render::Vertex3>& out_vertices,
		std::vector<render::Vertex2>& out_uvs,
		std::vector<render::Vertex3>& out_normals
	);

	struct IndexedModel
	{
		template<class T>
		struct IndexedBuffer
		{
			std::vector<T> data{};
			std::vector<uint32_t> indices{};
		};

		IndexedBuffer<render::Vertex3> vertices{};
		IndexedBuffer<render::Vertex2> uvs{};
		IndexedBuffer<render::Vertex3> normals{};
	};

	std::optional<IndexedModel> loadOBJ_indexed(
		std::unique_ptr<Buffer> const& buffer
	);
}
