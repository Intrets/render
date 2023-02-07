// render - A C++ OpenGL library
// Copyright (C) 2021 intrets

#pragma once

#include <cassert>
#include <optional>
#include <vector>

#include <wrangled_gl/wrangled_gl.h>

#include <tepp/dest.h>
#include <tepp/enum_bitflags.h>

#include <wglm/mat3x3.hpp>
#include <wglm/mat4x4.hpp>
#include <wglm/vec2.hpp>
#include <wglm/vec3.hpp>
#include <wglm/vec4.hpp>

#include <misc/Logger.h>

#include <mem/Global.h>
#include <mem/LazyGlobal.h>

#include "BufferWrappers.h"
#include "Colors.h"
#include "GLStateWrapper.h"

namespace render
{
	struct Vertex2
	{
		static constexpr auto member_count = 1;

		glm::vec2 v;
	};

	struct Vertex3
	{
		static constexpr auto member_count = 1;

		glm::vec3 v;
	};

	struct Vertex4
	{
		static constexpr auto member_count = 1;

		glm::vec4 v;
	};

	struct Index
	{
		static constexpr auto member_count = 1;

		uint16_t v;
	};

	template<class T, class phantom = void>
	struct UniformBase
	{
		std::optional<T> storage = std::nullopt;
		std::string name{};
		te::optional_ref<bwo::Program> program{};

		static constexpr inline GLint invalid_location = -1;
		GLint location = invalid_location;

		UniformBase(T val)
		    : storage(std::move(val)) {
		}
		UniformBase(std::string_view name_)
		    : name(name_) {
		}

		void init(bwo::Program& program_) {
			assert(!this->name.empty());
			assert(this->location == invalid_location);
			this->location = glGetUniformLocation(program_.ID, this->name.c_str());
			this->program = program_;
			// assert(this->location != invalid_location);
		};

		void setFromOther(UniformBase<T, phantom> const& other) {
			// assert(this->location != invalid_location);
			// assert(other.location == invalid_location);
			assert(this->program.has_value() && this->program.value().isBound());

			if (other.storage.has_value()) {
				this->setFromOtherImpl(other.storage.value());
			}
		}

		inline void setFromOtherImpl(T const& other);

		UniformBase() = delete;
		~UniformBase() = default;

		DEFAULT_COPY_MOVE(UniformBase);
	};

	using Uniform1f = UniformBase<float>;
	template<>
	inline void Uniform1f::setFromOtherImpl(float const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		glUniform1f(this->location, other);
	}

	using Uniform2f = UniformBase<glm::vec2>;
	template<>
	inline void Uniform2f::setFromOtherImpl(glm::vec2 const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		glUniform2fv(this->location, 1, &other[0]);
	}

	using Uniform3f = UniformBase<glm::vec3>;
	template<>
	inline void Uniform3f::setFromOtherImpl(glm::vec3 const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		glUniform3fv(this->location, 1, &other[0]);
	}

	using Uniform4f = UniformBase<glm::vec4>;
	template<>
	inline void Uniform4f::setFromOtherImpl(glm::vec4 const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		glUniform4fv(this->location, 1, &other[0]);
	}

	using Uniform1fv = UniformBase<std::vector<float>>;
	template<>
	inline void Uniform1fv::setFromOtherImpl(std::vector<float> const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		glUniform1fv(this->location, static_cast<GLsizei>(other.size()), other.data());
	}

	using Uniform3fv = UniformBase<std::vector<glm::vec3>>;
	template<>
	inline void Uniform3fv::setFromOtherImpl(std::vector<glm::vec3> const& other) {
		assert(this->program.has_value() && this->program.value().isBound());
		assert(!other.empty());

		glUniform3fv(this->location, static_cast<GLsizei>(other.size()), &other[0][0]);
	}

	using Uniform4fv = UniformBase<std::vector<glm::vec4>>;
	template<>
	inline void Uniform4fv::setFromOtherImpl(std::vector<glm::vec4> const& other) {
		assert(this->program.has_value() && this->program.value().isBound());
		assert(!other.empty());

		glUniform4fv(this->location, static_cast<GLsizei>(other.size()), &other[0][0]);
	}

	namespace description
	{
		struct BufferTexture;
		struct Texture2D;
		struct Texture2DArray;
		struct Texture3D;
		struct TexturesBound;
	}

	using UniformBufferTexture = UniformBase<GLuint, description::BufferTexture>;
	using UniformTexture2D = UniformBase<GLuint, description::Texture2D>;
	using UniformTexture2DArray = UniformBase<GLuint, description::Texture2DArray>;
	using UniformTexture3D = UniformBase<GLuint, description::Texture3D>;

	template<>
	inline void UniformBufferTexture::setFromOtherImpl(GLuint const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		auto unit = *LazyGlobal<int, description::TexturesBound>;

		glActiveTexture(GL_TEXTURE0 + *unit);
		glBindTexture(GL_TEXTURE_BUFFER, other);
		glUniform1i(this->location, *unit);

		(*unit)++;
	}

	template<>
	inline void UniformTexture2D::setFromOtherImpl(GLuint const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		auto unit = *LazyGlobal<int, description::TexturesBound>;

		glActiveTexture(GL_TEXTURE0 + *unit);
		glBindTexture(GL_TEXTURE_2D, other);
		glUniform1i(this->location, *unit);

		(*unit)++;
	}

	template<>
	inline void UniformTexture2DArray::setFromOtherImpl(GLuint const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		auto unit = *LazyGlobal<int, description::TexturesBound>;

		glActiveTexture(GL_TEXTURE0 + *unit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, other);
		glUniform1i(this->location, *unit);

		(*unit)++;
	}

	template<>
	inline void UniformTexture3D::setFromOtherImpl(GLuint const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		auto unit = *LazyGlobal<int, description::TexturesBound>;

		glActiveTexture(GL_TEXTURE0 + *unit);
		glBindTexture(GL_TEXTURE_3D, other);
		glUniform1i(this->location, *unit);

		(*unit)++;
	}

	using UniformMatrix4f = UniformBase<glm::mat4>;
	template<>
	inline void UniformMatrix4f::setFromOtherImpl(glm::mat4 const& other) {
		assert(this->program.has_value() && this->program.value().isBound());

		glUniformMatrix4fv(this->location, 1, GL_FALSE, &other[0][0]);
	}

	enum class BufferUsageHint
	{
		STREAM_DRAW,
		STREAM_READ,
		STREAM_COPY,
		STATIC_DRAW,
		STATIC_READ,
		STATIC_COPY,
		DYNAMIC_DRAW,
		DYNAMIC_READ,
		DYNAMIC_COPY,
	};

	constexpr static GLenum convert(BufferUsageHint hint) {
		constexpr GLenum lookup[] = {
			GL_STREAM_DRAW,
			GL_STREAM_READ,
			GL_STREAM_COPY,
			GL_STATIC_DRAW,
			GL_STATIC_READ,
			GL_STATIC_COPY,
			GL_DYNAMIC_DRAW,
			GL_DYNAMIC_READ,
			GL_DYNAMIC_COPY,
		};

		return lookup[static_cast<size_t>(hint)];
	}

	template<class T, int divisor_>
	struct ArrayBuffer
	{
		using value_type = T;
		constexpr static int divisor = divisor_;

		GLuint ID = 0;
		size_t size = 0;

		ArrayBuffer() {
			assert(this->ID == 0);
			glGenBuffers(1, &this->ID);
		}

		~ArrayBuffer() {
			assert(this->ID != 0);
			glDeleteBuffers(1, &this->ID);
		}

		void set(std::vector<T> const& data, BufferUsageHint hint, GLenum location = GL_ARRAY_BUFFER) {
			this->setRaw(sizeof(T) * data.size(), data.data(), hint, location);
		};

		void bind(GLenum location) {
			assert(this->ID != 0);
			glBindBuffer(location, this->ID);
		}

		void setRaw(size_t size_, void const* data, BufferUsageHint hint, GLenum location = GL_ARRAY_BUFFER) {
			if (size_ == 0) {
				this->size = 0;
				return;
			}

			assert(this->ID != 0);
			assert(size_ > 0);
			this->size = size_ / sizeof(T);
			glBindBuffer(location, this->ID);
			glBufferData(location, size_, data, convert(hint));
		}

		NO_COPY(ArrayBuffer);
	};

	struct VertexInfoState
	{
		GLuint index = 0;
		GLsizei stride = 0;
		std::intptr_t offset = 0;
		GLuint divisor = 0;
	};

	namespace detail
	{
		template<class T, size_t size>
		struct applyVertexInfo;

		template<size_t size>
		struct applyVertexInfo<float, size>
		{
			static void run(VertexInfoState& state) {
				glVertexAttribPointer(
				    state.index,
				    static_cast<GLint>(size),
				    GL_FLOAT,
				    GL_FALSE,
				    state.stride,
				    (void*)state.offset
				);
				glVertexAttribDivisor(state.index, state.divisor);
				glEnableVertexAttribArray(state.index);

				state.index++;
				state.offset += sizeof(float) * size;
			}
		};

		template<size_t size>
		struct applyVertexInfo<int32_t, size>
		{
			static void run(VertexInfoState& state) {
				glVertexAttribIPointer(
				    state.index,
				    static_cast<GLint>(size),
				    GL_INT,
				    state.stride,
				    (void*)state.offset
				);
				glVertexAttribDivisor(state.index, state.divisor);
				glEnableVertexAttribArray(state.index);

				state.index++;
				state.offset += sizeof(int32_t) * size;
			}
		};

		template<size_t size>
		struct applyVertexInfo<Color, size>
		{
			static void run(VertexInfoState& state) {
				// used as a vec4 in shader, makes no sense to have multiple
				static_assert(size == 1);
				glVertexAttribPointer(
				    state.index,
				    static_cast<GLint>(4),
				    GL_UNSIGNED_BYTE,
				    GL_TRUE,
				    state.stride,
				    (void*)state.offset
				);
				glVertexAttribDivisor(state.index, state.divisor);
				glEnableVertexAttribArray(state.index);

				state.index++;
				state.offset += sizeof(Color) * size;
			}
		};

		template<size_t size>
		struct applyVertexInfo<glm::mat3, size>
		{
			static void run(VertexInfoState& state) {
				static_assert(size == 1);
				for (size_t i = 0; i < 3; i++) {
					glVertexAttribPointer(
					    state.index,
					    static_cast<GLint>(3),
					    GL_FLOAT,
					    GL_FALSE,
					    state.stride,
					    (void*)state.offset
					);
					glVertexAttribDivisor(state.index, state.divisor);
					glEnableVertexAttribArray(state.index);

					state.index++;
					state.offset += sizeof(float) * 3;
				}
			}
		};

		template<size_t size>
		struct applyVertexInfo<glm::mat4, size>
		{
			static void run(VertexInfoState& state) {
				static_assert(size == 1);
				for (size_t i = 0; i < 4; i++) {
					glVertexAttribPointer(
					    state.index,
					    static_cast<GLint>(4),
					    GL_FLOAT,
					    GL_FALSE,
					    state.stride,
					    (void*)state.offset
					);
					glVertexAttribDivisor(state.index, state.divisor);
					glEnableVertexAttribArray(state.index);

					state.index++;
					state.offset += sizeof(float) * 4;
				}
			}
		};
	}

	template<class T>
	static void applyVertexInfo(VertexInfoState& state) {
		if constexpr (std::same_as<T, int32_t>) {
			detail::applyVertexInfo<int32_t, 1>::run(state);
		}
		else if constexpr (std::same_as<T, float>) {
			detail::applyVertexInfo<float, 1>::run(state);
		}
		else if constexpr (std::same_as<T, glm::vec2>) {
			detail::applyVertexInfo<float, 2>::run(state);
		}
		else if constexpr (std::same_as<T, glm::vec3>) {
			detail::applyVertexInfo<float, 3>::run(state);
		}
		else if constexpr (std::same_as<T, glm::vec4>) {
			detail::applyVertexInfo<float, 4>::run(state);
		}
		else if constexpr (std::same_as<T, Color>) {
			detail::applyVertexInfo<Color, 1>::run(state);
		}
		else if constexpr (std::same_as<T, glm::mat3>) {
			detail::applyVertexInfo<glm::mat3, 1>::run(state);
		}
		else if constexpr (std::same_as<T, glm::mat4>) {
			detail::applyVertexInfo<glm::mat4, 1>::run(state);
		}
		else {
			assert(0);
		}
	}

	struct ScopedVAO
	{
		bool resetOnDestruct = false;
		ogs::State& openglState;

		ScopedVAO() = delete;
		ScopedVAO(ogs::State& openglState, GLuint id, bool resetOnDestruct_);

		~ScopedVAO();

		NO_COPY_MOVE(ScopedVAO);
	};

	template<class... Buffers>
	struct VAO
	{
		GLuint id = 0;
		ogs::State& openglState;

		[[nodiscard]] ScopedVAO bind(bool resetOnDestruct = false) {
			return ScopedVAO(openglState, this->id, resetOnDestruct);
		}

		VAO(ogs::State& openglState_, std::tuple<Buffers...>& buffers)
		    : openglState(openglState_) {
			glGenVertexArrays(1, &this->id);

			auto bind = this->bind();

			VertexInfoState state{
				.index = 0,
			};

			te::tuple_for_each(
			    [&](auto& buffer) {
				    using T = std::remove_cvref_t<decltype(buffer)>;
				    using members = te::get_members_t<typename T::value_type>;

				    state.stride = sizeof(typename T::value_type);
				    state.offset = 0;
				    state.divisor = buffer.divisor;

				    buffer.bind(GL_ARRAY_BUFFER);

				    logger->logInfo("new attribute group\n");
				    te::for_each_type([&]<class T>(te::Type_t<T>) {
					    logger->logInfo(
					        "new attribute {}, size {}, stride {}, offset {}\n", state.index, sizeof(T), state.stride, state.offset
					    );

					    applyVertexInfo<T>(state);
				    },
				                      te::Type<members>);

				    assert(state.stride == state.offset);
			    },
			    buffers
			);
		}

		VAO() = delete;
		NO_COPY_MOVE(VAO);
		~VAO() {
			glDeleteVertexArrays(1, &this->id);
		}
	};

	template<class T>
	struct RenderInfoBase
	{
		std::vector<T> data;

		std::span<T> resizeAdditional(int i) {
			assert(i >= 0);
			auto current = this->data.size();
			this->data.resize(this->data.size() + i);

			return std::span(this->data.begin() + current, i);
		}

		void add(T&& t) {
			this->data.push_back(std::forward<T>(t));
		}

		void add(T const& t) {
			this->data.push_back(t);
		}

		size_t getSize() const {
			return this->data.size();
		}

		T* getData() const {
			return this->data.data();
		}
	};

	template<class T>
	struct RenderInfoTemplate;

	enum RenderMode
	{
		TRIANGLE = 1 << 0,
		POINT = 1 << 1,
		LINE = 1 << 2,
		LINESTRIP = 1 << 3,
		TRIANGLESTRIP = 1 << 4,
	};

	static constexpr auto getRenderMode(int mode) {
		switch (mode) {
			case render::TRIANGLE:
				return GL_TRIANGLES;
				break;
			case render::POINT:
				return GL_POINTS;
				break;
			case render::LINE:
				return GL_LINES;
				break;
			case render::LINESTRIP:
				return GL_LINE_STRIP;
				break;
			case render::TRIANGLESTRIP:
				return GL_TRIANGLE_STRIP;
				break;
			default:
				assert(0);
				return GL_TRIANGLES;
				break;
		}
	}

	static constexpr auto all_render_modes = te::value_list<render::TRIANGLE, render::POINT, render::LINE, render::LINESTRIP, render::TRIANGLESTRIP>;

	template<class Uniforms, int mode, class... Buffers>
	struct Renderer2
	{
		struct RendererVAO
		{
			std::tuple<Buffers...> buffers;
			VAO<Buffers...> vao;

			RendererVAO(ogs::State& openglState)
			    : vao(openglState, this->buffers) {
			}

			auto bind(bool resetOnDestruct = false) {
				return this->vao.bind(resetOnDestruct);
			}

			template<class T>
			RendererVAO& setBuffer(RenderInfoTemplate<T> const& info, BufferUsageHint hint) {
				constexpr auto enumerated_buffer_types = te::enumerate_in_list(
				    te::List<typename Buffers::value_type...>
				);

				constexpr auto same_types = te::filter(
				    []<template<class...> class L, class S, auto I>(te::Type_t<L<S, te::detail::Value<I>>>) {
					    return te::Value<std::same_as<T, S>>;
				    },
				    enumerated_buffer_types
				);

				static_assert(Getvalue(te::list_size(same_types)) != 0, "No buffer found with type T");
				static_assert(Getvalue(te::list_size(same_types)) < 2, "Multiple buffers found with type T");

				constexpr size_t I = std::tuple_element_t<1, std::tuple_element_t<0, Gettype(same_types)>>::value;

				this->setBuffer<I>(info, hint);

				return *this;
			}

			template<size_t I>
			RendererVAO& setBuffer(RenderInfoTemplate<std::tuple_element_t<I, std::tuple<typename Buffers::value_type...>>> const& info, BufferUsageHint hint) {
				std::get<I>(this->buffers).set(info.data, hint);

				return *this;
			}
		};

		struct RendererProgram
		{
			bwo::Program program;
			Uniforms uniforms;

			RendererProgram(
			    ogs::State& openglState,
			    bwo::Program::BufferGenerator vertexGenerator,
			    bwo::Program::BufferGenerator fragmentGenerator,
			    std::string_view description
			);

			void render(
			    bwo::FrameBuffer& target,
			    glm::ivec4 viewport,
			    ogs::Configuration const& config,
			    Uniforms const& uniforms_,
			    RendererVAO& vao
			);

			void setUniforms(Uniforms const& uniforms_);
		};
	};

	template<class Uniforms, class... Buffers>
	using TriRenderer = Renderer2<Uniforms, RenderMode::TRIANGLE, Buffers...>;

	template<class Uniforms, class... Buffers>
	using PointRenderer = Renderer2<Uniforms, RenderMode::POINT, Buffers...>;

	template<class Uniforms, class... Buffers>
	using LineRenderer = Renderer2<Uniforms, RenderMode::LINE, Buffers...>;

	template<class Uniforms, class... Buffers>
	using LinePointRenderer = Renderer2<Uniforms, RenderMode::LINESTRIP, Buffers...>;

	template<>
	struct render::RenderInfoTemplate<Vertex2> : RenderInfoBase<Vertex2>
	{
		void addVertex(glm::vec2 v) {
			this->data.push_back({ v });
		}
	};

	template<>
	struct render::RenderInfoTemplate<Vertex3> : RenderInfoBase<Vertex3>
	{
		void addVertex(glm::vec3 v) {
			this->data.push_back({ v });
		}
	};

	template<>
	struct render::RenderInfoTemplate<Vertex4> : RenderInfoBase<Vertex4>
	{
		void addVertex(glm::vec4 v) {
			this->data.push_back({ v });
		}
	};

	template<>
	struct render::RenderInfoTemplate<Index> : RenderInfoBase<Index>
	{
		void addIndex(uint16_t i) {
			this->data.push_back({ i });
		}
	};

	template<class Uniforms, int mode, class... Buffers>
	inline Renderer2<Uniforms, mode, Buffers...>::RendererProgram::RendererProgram(
	    ogs::State& openglState,
	    bwo::Program::BufferGenerator vertexGenerator,
	    bwo::Program::BufferGenerator fragmentGenerator,
	    std::string_view description
	)
	    : program(openglState, vertexGenerator, fragmentGenerator, description) {
		if constexpr (Uniforms::member_count > 0) {
			te::tuple_for_each(
			    [this](auto& t) {
				    t.init(this->program);
			    },
			    te::get_tie(this->uniforms)
			);
		}
	}

	template<class Uniforms, int mode, class... Buffers>
	inline void Renderer2<Uniforms, mode, Buffers...>::RendererProgram::render(
	    bwo::FrameBuffer& target,
	    glm::ivec4 viewport,
	    ogs::Configuration const& config,
	    Uniforms const& uniforms_,
	    RendererVAO& vao
	) {
		// Find instance and element count
		std::optional<int> instanceCount;
		int elementCount;
		{
			std::optional<int> maybeElementCount;

			te::tuple_for_each([&](auto& buffer) {
				if constexpr (buffer.divisor == 0) {
					if (maybeElementCount.has_value()) {
						assert(maybeElementCount.value() == buffer.size);
					}
					else {
						maybeElementCount = static_cast<int>(buffer.size);
					}
				}
				else {
					auto bufferSize = static_cast<int>(buffer.size);

					if (!instanceCount.has_value()) {
						instanceCount = bufferSize;
					}
					else if (instanceCount.value() != bufferSize) {
						logger->logError("Buffer size mis-match when rendering program {}. Sizes {} and {}. Continuing with smallest.\n", this->program.getDescription(), instanceCount.value(), bufferSize);

						instanceCount = std::min(instanceCount.value(), bufferSize);
					}
				}
			},
			                   vao.buffers);

			if (!maybeElementCount.has_value()) {
				logger->logError("Missing element buffer to determine amount of elements to draw in program {}. Skipping render.\n", this->program.getDescription());
				return;
			}
			elementCount = maybeElementCount.value();
		}

		if (elementCount == 0 || (instanceCount.has_value() && instanceCount.value() == 0)) {
			return;
		}

		this->program.openglState.setConfiguration(config);

		auto useVAO = vao.bind();
		auto useProgram = this->program.bind();

		this->setUniforms(uniforms_);

		if (!instanceCount.has_value()) {
			te::for_each_type(
			    [&]<auto m>(te::Value_t<m>) {
				    if constexpr ((mode & m) == m) {
					    target.draw(
					        viewport,
					        [elementCount]() {
						        glDrawArrays(
						            getRenderMode(m),
						            0,
						            static_cast<GLsizei>(elementCount)
						        );
					        }
					    );
				    }
			    },
			    all_render_modes
			);
		}
		else {
			te::for_each_type(
			    [&]<auto m>(te::Value_t<m>) {
				    if constexpr ((mode & m) == m) {
					    target.draw(
					        viewport,
					        [elementCount, instanceCount = instanceCount.value()]() {
						        glDrawArraysInstanced(
						            getRenderMode(m),
						            0,
						            static_cast<GLsizei>(elementCount),
						            static_cast<GLsizei>(instanceCount)
						        );
					        }
					    );
				    }
			    },
			    all_render_modes
			);
		}
	}

	template<class Uniforms, int mode, class... Buffers>
	inline void Renderer2<Uniforms, mode, Buffers...>::RendererProgram::setUniforms(Uniforms const& uniforms_) {
		auto programUse = this->program.bind();

		auto targetUniforms = te::get_tie(this->uniforms);
		auto storageUniforms = te::get_tie(uniforms_);

		auto zipped = te::tuple_zip(targetUniforms, storageUniforms);

		auto unit = *LazyGlobal<int, description::TexturesBound>;
		*unit = 0;

		te::tuple_for_each(
		    [](auto& t) {
			    auto& [target, storage] = t;
			    target.setFromOther(storage);
		    },
		    zipped
		);
	}
}
