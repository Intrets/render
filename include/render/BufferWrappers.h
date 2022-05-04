// render - A C++ OpenGL library
// Copyright (C) 2022  Intrets
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

#include <wrangled_gl/wrangled_gl.h>

#include <wglm/vec2.hpp>
#include <wglm/vec3.hpp>
#include <wglm/mat4x4.hpp>

#include <optional>
#include <functional>
#include <filesystem>

#include <misc/Misc.h>

#include <resource_data/ResourceData.h>

#include <iostream>

#include "Colors.h"

namespace ogs
{
	class ProgramRegistry;
	class State;
}

namespace render
{
	namespace bwo
	{
		enum class BufferHint
		{
			STREAM_DRAW = GL_STREAM_DRAW,
			STREAM_READ = GL_STREAM_READ,
			STREAM_COPY = GL_STREAM_COPY,
			STATIC_DRAW = GL_STATIC_DRAW,
			STATIC_READ = GL_STATIC_READ,
			STATIC_COPY = GL_STATIC_COPY,
			DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
			DYNAMIC_READ = GL_DYNAMIC_READ,
			DYNAMIC_COPY = GL_DYNAMIC_COPY,
		};

		class Texture2DArray
		{
		public:
			GLuint ID;
			glm::ivec3 size;

			Texture2DArray(
				glm::ivec3 size,
				GLint level,
				GLenum internalFormat,
				GLenum magFilter, GLenum minFilter, GLenum wrapS, GLenum wrapT,
				GLint border,
				GLenum pixelFormat,
				GLenum type,
				void const* pixels
			);

			~Texture2DArray();

			Texture2DArray(Texture2DArray&& other);
			Texture2DArray& operator=(Texture2DArray&& other);

			NO_COPY(Texture2DArray);
		};

		class Texture2D
		{
		public:
			GLuint ID;
			glm::ivec2 size;

			Texture2D();
			Texture2D(GLuint handle);
			Texture2D(
				glm::ivec2 size,
				GLint level,
				GLenum internalFormat,
				GLenum magFilter, GLenum minFilter, GLenum wrapS, GLenum wrapT,
				GLint border,
				GLenum pixelFormat,
				GLenum type,
				void const* pixels
			);

			~Texture2D();

			Texture2D(Texture2D&& other);
			Texture2D& operator=(Texture2D&& other);

			NO_COPY(Texture2D);
		};

		namespace Texture2DHelper
		{
			Texture2D makeDepthBuffer(glm::ivec2 size);
			Texture2D makeNoFiltering(glm::ivec2 size);
			Texture2D makeLinearFiltering(glm::ivec2 size);
			Texture2D makeFloatBuffer(glm::ivec2 size, bool filtering = false);
		};

		namespace Texture2DArrayHelper
		{
			Texture2DArray makeLinearFiltering(glm::ivec3 size);
		}

		class FrameBuffer
		{
		private:
			ogs::State& openglState;

		public:
			GLuint ID = 0;
			glm::ivec2 size{};

			FrameBuffer(ogs::State& openglState);
			FrameBuffer(ogs::State& openglState, int x, int y);
			FrameBuffer(FrameBuffer&& other);
			FrameBuffer& operator=(FrameBuffer&& other);

			NO_COPY(FrameBuffer);
			~FrameBuffer();

			void bindTextureColor(int32_t attachmentNumber, bwo::Texture2D const& texture, GLint mipmap);
			void bindTextureColorLayer(int32_t attachmentNumber, bwo::Texture2DArray const& texture, int32_t mipmap, int32_t layer);
			void bindDepthLayer(bwo::Texture2D const& texture);

			void draw(glm::ivec4 viewport, std::function<void()> f);

			void clear(glm::vec4 color, bool depth);
			void clearDepth();

			void onlyDepth();
		};

		class Program
		{
		public:
			using BufferGenerator = std::function<std::optional<std::unique_ptr<Buffer>>()>;

			struct ScopedProgram
			{
				bool resetOnDestruct = false;
				ogs::State& openglState;

				ScopedProgram() = delete;
				ScopedProgram(ogs::State& openglState_, GLint id, bool resetOnDestruct_);

				~ScopedProgram();

				NO_COPY_MOVE(ScopedProgram);
			};

			GLint ID = 0;
			ogs::State& openglState;

		private:
			BufferGenerator getVertexBuffer;
			BufferGenerator getFragmentBuffer;

			std::string description{};

		public:
			[[nodiscard]]
			ScopedProgram bind(bool resetOnDestruct = false);
			[[nodiscard]]
			bool refreshShaders();
			bool isNull() const;
			bool isNotNull() const;
			bool isBound() const;

			Program() = delete;
			Program(ogs::State& openglState_, char const* vert_raw, size_t vert_size, char const* frag_raw, size_t frag_size, std::string const& description);
			Program(ogs::State& openglState_, BufferGenerator vertexGenerator, BufferGenerator fragmentGenerator, std::string_view description);

			Program(Program&& other);
			Program& operator=(Program&& other);

			std::string_view getDescription() const;

			NO_COPY(Program);
			~Program();
		};

		class Uniform2iv
		{
		private:
			GLuint location = 0;

		public:
			void set(glm::ivec2 vec);
			void set(std::vector<glm::ivec2> const& vecs);

			Uniform2iv() = default;
			Uniform2iv(std::string name, Program const& program);
			DEFAULT_COPY_MOVE(Uniform2iv);
			~Uniform2iv() = default;
		};

		class Uniform2fv
		{
		private:
			GLuint location = 0;

		public:
			void set(glm::vec2 vec);

			Uniform2fv() = default;
			Uniform2fv(std::string name, Program const& program);
			DEFAULT_COPY_MOVE(Uniform2fv);
			~Uniform2fv() = default;
		};

		class UniformMatrix4fv
		{
		private:
			GLuint location = 0;

		public:
			void set(glm::mat4 const& mat);

			UniformMatrix4fv() = default;
			UniformMatrix4fv(std::string name, Program const& program);
			DEFAULT_COPY_MOVE(UniformMatrix4fv);
			~UniformMatrix4fv() = default;
		};

		class UniformTexture2D
		{
		private:
			int32_t unit = 0;
			GLuint location = 0;

		public:
			void set(GLuint texture);
			void set(bwo::Texture2D const& texture);

			UniformTexture2D() = default;
			UniformTexture2D(std::string name, Program const& program, int32_t unit);
			DEFAULT_COPY_MOVE(UniformTexture2D);
			~UniformTexture2D() = default;
		};

		class Uniform3fv
		{
		private:
			GLuint location = 0;

		public:
			void set(glm::vec3 vec);

			Uniform3fv() = default;
			Uniform3fv(std::string name, Program const& program);
			DEFAULT_COPY_MOVE(Uniform3fv);
			~Uniform3fv() = default;
		};

		class Uniform4fv
		{
		private:
			GLuint location = 0;

		public:
			void set(glm::vec4 vec);

			Uniform4fv() = default;
			Uniform4fv(std::string name, Program const& program);
			DEFAULT_COPY_MOVE(Uniform4fv);
			~Uniform4fv() = default;
		};

		class Uniform1f
		{
		private:
			GLuint location = 0;

		public:
			void set(float f);

			Uniform1f() = default;
			Uniform1f(std::string name, Program const& program);
			DEFAULT_COPY_MOVE(Uniform1f);
			~Uniform1f() = default;
		};

		class UniformTexture2DArray
		{
		private:
			GLuint location = 0;
			int32_t unit = 0;

		public:
			void set(Texture2DArray const& texture);

			UniformTexture2DArray() = default;
			UniformTexture2DArray(std::string name, Program const& program, int32_t unit);
			DEFAULT_COPY_MOVE(UniformTexture2DArray);
			~UniformTexture2DArray() = default;
		};

		class Uniform1i
		{
		private:
			GLuint location = 0;

		public:
			void set(int32_t i);

			Uniform1i() = default;
			Uniform1i(std::string name, Program const& program);
			DEFAULT_COPY_MOVE(Uniform1i);
			~Uniform1i() = default;
		};
	}
}
