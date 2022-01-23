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

#include <wglm/glm.hpp>

#include <optional>
#include <functional>
#include <filesystem>

#include <misc/Misc.h>

#include <resource_data/ResourceData.h>

#include <iostream>

#include "Colors.h"

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

		template<class T>
		class ArrayBuffer
		{
		public:

		private:
			GLenum usageHint;

		public:
			GLuint ID = 0;

			ArrayBuffer() = default;
			ArrayBuffer(BufferHint hint);
			~ArrayBuffer();

			void set(std::vector<T> const& data);
			void set(uint32_t size, std::vector<T> const& data);
			void setRaw(uint32_t size, void const* data);

			void bind(GLenum location);

			ArrayBuffer(ArrayBuffer<T>&& other) {
				this->usageHint = other.usageHint;
				this->ID = other.ID;

				other.ID = 0;
				other.usageHint = {};
			}

			ArrayBuffer& operator=(ArrayBuffer<T>&& other) {
				if (this != &other) {
					if (this->ID != 0) {
						glDeleteBuffers(1, &this->ID);
					}

					this->usageHint = other.usageHint;
					this->ID = other.ID;

					other.ID = 0;
					other.usageHint = {};
				}
				return *this;
			}

			NO_COPY(ArrayBuffer);

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
			Texture2D makeMSLinearFiltering(glm::ivec2 size);
			Texture2D makeFloatBuffer(glm::ivec2 size, bool filtering = false);
		};

		namespace Texture2DArrayHelper
		{
			Texture2DArray makeLinearFiltering(glm::ivec3 size);
		}

		class FrameBuffer
		{
		public:
			GLuint ID = 0;
			glm::ivec2 size{};

			FrameBuffer();
			FrameBuffer(int x, int y);
			FrameBuffer(FrameBuffer&& other) {
				this->ID = other.ID;
				this->size = other.size;

				other.ID = 0;
				other.size = { 0,0 };
			}
			FrameBuffer& operator=(FrameBuffer&& other) {
				if (this != &other) {
					if (this->ID != 0) {
						glDeleteFramebuffers(1, &this->ID);
					}

					this->ID = other.ID;
					this->size = other.size;

					other.ID = 0;
					other.size = { 0,0 };
				}
				return *this;
			}
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
		private:
			friend class Uniform1f;
			friend class Uniform1i;
			friend class Uniform2fv;
			friend class Uniform2iv;
			friend class Uniform3fv;
			friend class Uniform3iv;
			friend class Uniform4fv;
			friend class Uniform4iv;
			friend class UniformMatrix4fv;
			friend class UniformTexture2D;
			friend class UniformTexture2DArray;

		public:
			using BufferGenerator = std::function<std::optional<std::unique_ptr<Buffer>>()>;

			struct ScopedUseProgram
			{
				inline static GLuint current = 0;

				ScopedUseProgram() = delete;
				ScopedUseProgram(GLuint id) {
					assert(current == 0);
					current = id;
					glUseProgram(id);
				}

				~ScopedUseProgram() {
					current = 0;
					glUseProgram(0);
				}

				NO_COPY_MOVE(ScopedUseProgram);
			};

			GLuint ID = 0;

		private:
			BufferGenerator getVertexBuffer;
			BufferGenerator getFragmentBuffer;

			std::string description{};

		public:
			static std::unordered_map<int32_t, Program*> refs;
			static std::string listAll();
			static std::optional<Program const*> lookup(int32_t id);
			static void change(GLuint ID, Program& to);
			static void addProgram(Program& program);
			static void deleteProgram(Program& program);
			[[nodiscard]]
			static bool refreshAll();

			[[nodiscard]]
			ScopedUseProgram getScopedUse();
			[[nodiscard]]
			bool refreshShaders();

			Program() = default;
			Program(char const* vert_raw, size_t vert_size, char const* frag_raw, size_t frag_size, std::string const& description);
			Program(std::string_view vert_name, std::string_view frag_file, std::string const& description, int);
			Program(BufferGenerator vertexGenerator, BufferGenerator fragmentGenerator, std::string_view description);

			Program(Program&& other) {
				change(other.ID, *this);

				this->ID = other.ID;
				this->description = other.description;

				this->getFragmentBuffer = other.getFragmentBuffer;
				this->getVertexBuffer = other.getVertexBuffer;

				other.ID = 0;
				other.description = {};

				other.getFragmentBuffer = {};
				other.getVertexBuffer = {};
			};

			Program& operator=(Program&& other) {
				if (this != &other) {
					if (this->ID != 0) {
						deleteProgram(*this);
					}
					change(other.ID, *this);

					this->ID = other.ID;
					this->description = other.description;

					this->getFragmentBuffer = other.getFragmentBuffer;
					this->getVertexBuffer = other.getVertexBuffer;

					other.ID = 0;
					other.description = {};

					other.getFragmentBuffer = {};
					other.getVertexBuffer = {};
				}
				return *this;
			};

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

		struct Model
		{
			bwo::ArrayBuffer<glm::vec3> model{ bwo::BufferHint::STATIC_DRAW };
			bwo::ArrayBuffer<glm::vec2> uv{ bwo::BufferHint::STATIC_DRAW };
			bwo::ArrayBuffer<glm::vec3> normals{ bwo::BufferHint::STATIC_DRAW };
			bwo::ArrayBuffer<uint16_t> indices{ bwo::BufferHint::STATIC_DRAW };
			int32_t indexSize{};
		};


		template<class T>
		inline ArrayBuffer<T>::ArrayBuffer(BufferHint hint) {
			assert(this->ID == 0);
			glGenBuffers(1, &this->ID);
			this->usageHint = static_cast<GLenum>(hint);
		}

		template<class T>
		inline ArrayBuffer<T>::~ArrayBuffer() {
			glDeleteBuffers(1, &this->ID);
		}

		template<class T>
		inline void ArrayBuffer<T>::set(std::vector<T> const& data) {
			this->setRaw(static_cast<uint32_t>(sizeof(T) * data.size()), &data[0]);
		}

		template<class T>
		inline void ArrayBuffer<T>::set(uint32_t size, std::vector<T> const& data) {
			assert(size <= data.size());
			this->setRaw(static_cast<uint32_t>(sizeof(T) * size), &data[0]);
		}

		template<class T>
		inline void ArrayBuffer<T>::setRaw(uint32_t size, void const* data) {
			assert(this->ID != 0);
			glBindBuffer(GL_ARRAY_BUFFER, this->ID);
			glBufferData(GL_ARRAY_BUFFER, size, data, this->usageHint);
		}

		template<class T>
		inline void ArrayBuffer<T>::bind(GLenum location) {
			assert(this->ID != 0);
			glBindBuffer(location, this->ID);
		}
	}
}
