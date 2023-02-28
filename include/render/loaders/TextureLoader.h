// render - A C++ OpenGL library
// Copyright (C) 2021 intrets

#pragma once

#include <string>
#include <filesystem>

#include <resource_data/ResourceData.h>

namespace render
{
	namespace bwo
	{
		class Texture2D;
	}

	bwo::Texture2D load2DTexture(std::string const Filename);
	bwo::Texture2D load2DTexture(std::filesystem::path path);
	bwo::Texture2D load2DTexture(char const* data, size_t size);
	bwo::Texture2D load2DTexture(std::unique_ptr<Buffer> const& buffer);
	bwo::Texture2D load2DTexture(Buffer const& buffer);
}
