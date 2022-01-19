// The Happy Bunny License(Modified MIT License)
// --------------------------------------------------------------------------------
// Copyright(c) 2005 - 2014 G - Truc Creation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright noticeand this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Restrictions :
// 	By making use of the Software for military purposes, you choose to make a
// 	Bunny unhappy.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "loaders/TextureLoader.h"

#include <wrangled_gl/wrangled_gl.h>

#include <misc/Logger.h>

#include <iostream>

#if defined(COMPILER_CLANGCL) || defined(COMPILER_CLANG)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wignored-qualifiers"
	#pragma clang diagnostic ignored "-Wdeprecated-anon-enum-enum-conversion"
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(COMPILER_MSVC)
	#pragma warning(push,0)
#endif

#include <gli/gli.hpp>

#if defined(COMPILER_CLANGCL) || defined(COMPILER_CLANG)
	#pragma clang diagnostic pop
#elif defined(COMPILER_MSVC)
	#pragma warning(pop)
#endif

#include "BufferWrappers.h"

namespace render
{
	bwo::Texture2D load2DTexture(gli::texture Texture) {
		gli::gl GL(gli::gl::PROFILE_GL33);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
		GLenum Target = GL.translate(Texture.target());

		GLuint TextureName = 0;
		glGenTextures(1, &TextureName);
		glBindTexture(Target, TextureName);
		glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
		glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

		glm::tvec3<GLsizei> const Extent1(Texture.extent());
		GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

		switch (Texture.target()) {
			case gli::TARGET_1D_ARRAY:
			case gli::TARGET_2D:
			case gli::TARGET_CUBE:
				glTexStorage2D(
					Target, static_cast<GLint>(Texture.levels()), Format.Internal,
					Extent1.x, Texture.target() == gli::TARGET_2D ? Extent1.y : FaceTotal);
				break;
			case gli::TARGET_1D:
			case gli::TARGET_2D_ARRAY:
			case gli::TARGET_3D:
			case gli::TARGET_CUBE_ARRAY:
			default:
				assert(0);
				break;
		}

		glm::ivec2 size;

		size.x = Extent1.x;
		size.y = Texture.target() == gli::TARGET_2D ? Extent1.y : FaceTotal;

		for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer) {
			for (std::size_t Face = 0; Face < Texture.faces(); ++Face) {
				for (std::size_t Level = 0; Level < Texture.levels(); ++Level) {
					GLsizei const LayerGL = static_cast<GLsizei>(Layer);
					glm::tvec3<GLsizei> Extent(Texture.extent(Level));
					Target = gli::is_target_cube(Texture.target())
						? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
						: Target;


					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage2D(
							Target, static_cast<GLint>(Level),
							0, 0,
							Extent.x,
							Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage2D(
							Target, static_cast<GLint>(Level),
							0, 0,
							Extent.x,
							Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
							Format.External, Format.Type,
							Texture.data(Layer, Face, Level));
				}
			}
		}
		bwo::Texture2D res{ TextureName };
		res.size = size;
		return res;

	}

	bwo::Texture2D load2DTexture(std::string const Filename) {
		gli::texture Texture = gli::load(Filename);
		if (Texture.empty()) {
			logger->acquire()->log(Logger::Level::error, "Failed to load file {}\n", Filename);
			return 0;
		}

		return load2DTexture(Texture);
	}

	bwo::Texture2D load2DTexture(std::filesystem::path path) {
		return load2DTexture(path.string());
	}

	bwo::Texture2D load2DTexture(char const* data, size_t size) {
		gli::texture Texture = gli::load_dds(data, size);

		if (Texture.empty()) {
			logger->acquire()->log(Logger::Level::error, "Failed to load texture from raw data of size {}\n", size);
			return 0;
		}

		return load2DTexture(Texture);
	}

	bwo::Texture2D load2DTexture(std::unique_ptr<Buffer> const& buffer) {
		return load2DTexture(buffer->getData<char>(), buffer->getSize<char>());
	}
}
