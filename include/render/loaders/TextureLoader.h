// render - A C++ OpenGL library
// Copyright (C) 2021  Intrets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
