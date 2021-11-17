#pragma once

#include <string>
#include <filesystem>

namespace render
{
	namespace bwo
	{
		class Texture2D;
	}

	bwo::Texture2D load2DTexture(std::string const Filename);
	bwo::Texture2D load2DTexture(std::filesystem::path path);
	bwo::Texture2D load2DTexture(char const* data, size_t size);
}
