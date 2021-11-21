#pragma once

#include <vector>
#include <cassert>

#include <GL/glew.h>

#include <tepp/dest.h>

#include <wglm/glm.hpp>

#include <mem/LazyGlobal.h>
#include <mem/Global.h>

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
		T storage;
		std::string name;

		GLuint location = -1;

		UniformBase(T val) : storage(val) {}
		UniformBase(std::string_view name_) : name(name_) {}

		void init(bwo::Program const& program) {
			assert(!this->name.empty());
			assert(this->location == -1);
			this->location = glGetUniformLocation(program.ID, this->name.c_str());
			assert(this->location != -1);
		};

		void setFromOther(UniformBase<T, phantom> const& other) {
			assert(this->location != -1);
			assert(other.location == -1);

			setFromOtherImpl(other.storage);
		}

		inline void setFromOtherImpl(T other);

		UniformBase() = delete;
		~UniformBase() = default;

		DEFAULT_COPY_MOVE(UniformBase);
	};

	using Uniform1f = UniformBase<float>;
	template<>
	inline void Uniform1f::setFromOtherImpl(float other) {
		glUniform1f(this->location, other);
	}

	using Uniform2f = UniformBase<glm::vec2>;
	template<>
	inline void Uniform2f::setFromOtherImpl(glm::vec2 other) {
		glUniform2fv(this->location, 1, &other[0]);
	}

	namespace description
	{
		struct Texture2D;
		struct Texture2DArray;
		struct Texture3D;
		struct TexturesBound;
	}

	using UniformTexture2D = UniformBase<GLuint, description::Texture2D>;
	using UniformTexture2DArray = UniformBase<GLuint, description::Texture2DArray>;
	using UniformTexture3D = UniformBase<GLuint, description::Texture3D>;

	template<>
	inline void UniformTexture2D::setFromOtherImpl(GLuint other) {
		auto unit = *LazyGlobal<int, description::TexturesBound>;

		glActiveTexture(GL_TEXTURE0 + *unit);
		glBindTexture(GL_TEXTURE_2D, other);
		glUniform1i(this->location, *unit);

		(*unit)++;
	}

	template<>
	inline void UniformTexture2DArray::setFromOtherImpl(GLuint other) {
		auto unit = *LazyGlobal<int, description::TexturesBound>;

		glActiveTexture(GL_TEXTURE0 + *unit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, other);
		glUniform1i(this->location, *unit);

		(*unit)++;
	}

	template<>
	inline void UniformTexture3D::setFromOtherImpl(GLuint other) {
		auto unit = *LazyGlobal<int, description::TexturesBound>;

		glActiveTexture(GL_TEXTURE0 + *unit);
		glBindTexture(GL_TEXTURE_3D, other);
		glUniform1i(this->location, *unit);

		(*unit)++;
	}

	using UniformMatrix4f = UniformBase<glm::mat4>;
	template<>
	inline void UniformMatrix4f::setFromOtherImpl(glm::mat4 other) {
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

		void set(std::vector<T> const& data, BufferUsageHint hint) {
			this->setRaw(sizeof(T) * data.size(), data.data(), hint);
		};

		void bind(GLenum location) {
			assert(this->ID != 0);
			glBindBuffer(location, this->ID);
		}

		void setRaw(size_t size_, void const* data, BufferUsageHint hint) {
			assert(this->ID != 0);
			assert(size_ > 0);
			this->size = size_ / sizeof(T);
			glBindBuffer(GL_ARRAY_BUFFER, this->ID);
			glBufferData(GL_ARRAY_BUFFER, size_, data, convert(hint));
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
					static_cast<GLint>(size),
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
	}

	template<class T>
	static void applyVertexInfo(VertexInfoState& state) {
		if constexpr (std::same_as<T, float>) {
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
		else {
			static_assert(0);
		}
	}

	template<class... Buffers>
	struct VAO
	{
		GLuint ID = 0;

		struct BindScoped
		{
			BindScoped() = delete;
			BindScoped(GLuint ID) {
				assert(ID != 0);
				glBindVertexArray(ID);
			}

			~BindScoped() {
				glBindVertexArray(0);
			}

			NO_COPY_MOVE(BindScoped);
		};

		[[nodiscard]]
		BindScoped bindScoped() {
			return BindScoped(this->ID);
		}

		VAO(std::tuple<Buffers...>& buffers) {
			glGenVertexArrays(1, &this->ID);

			auto bind = this->bindScoped();

			VertexInfoState state{
				.index = 0,
			};

			te::tuple_for_each(
				[&](auto& buffer) {
					using T = std::remove_cvref_t<decltype(buffer)>;
					using members = te::get_members_t<T::value_type>;

					state.stride = sizeof(T::value_type);
					state.offset = 0;
					state.divisor = buffer.divisor;

					buffer.bind(GL_ARRAY_BUFFER);

					te::for_each_type([&]<class T>(te::Type_t<T>) {
						std::cout << "   new attribute\n";
						std::cout
							<< " attribute: " << state.index
							<< " size: " << sizeof(T)
							<< " stride: " << state.stride
							<< " offset: " << state.offset << "\n";

						applyVertexInfo<T>(state);
					}, te::Type<members>);

					assert(state.stride == state.offset);
				},
				buffers);
		}

		VAO() = delete;
		NO_COPY_MOVE(VAO);
		~VAO() {
			glDeleteVertexArrays(1, &this->ID);
		}
	};

	template<class T>
	struct RenderInfoBase
	{
		std::vector<T> data;

		size_t getSize() const {
			return this->data.size();
		}

		T* getData() const {
			return this->data.data();
		}
	};

	template<class T>
	struct RenderInfoTemplate;

	template<
		class Uniforms,
		class... Buffers
	> struct Renderer
	{
		bwo::Program program;

		std::tuple<Buffers...> buffers;
		VAO<Buffers...> vao;

		Uniforms uniforms;

		Renderer(
			bwo::Program::BufferGenerator vertexGenerator,
			bwo::Program::BufferGenerator fragmentGenerator,
			std::string_view description
		) : program(vertexGenerator, fragmentGenerator, description),
			vao(buffers) {

			te::tuple_for_each(
				[this](auto& t) {
					t.init(this->program);
				}, te::get_tie(this->uniforms));
		}

		template<class T>
		void setBuffer(RenderInfoTemplate<T> const& info, BufferUsageHint hint) {
			constexpr auto enumerated_buffer_types = te::enumerate_in_list(
				te::List<typename Buffers::value_type...>
			);

			constexpr auto same_types = te::filter(
				[]<template<class...> class L, class S, auto I>(te::Type_t<L<S, te::detail::Value<I>>>) {
				return te::Value<std::same_as<T, S>>;
			}, enumerated_buffer_types);

			static_assert(Getvalue(te::list_size(same_types)) != 0, "No buffer found with type T");
			static_assert(Getvalue(te::list_size(same_types)) < 2, "Multiple buffers found with type T");

			constexpr size_t I = std::tuple_element_t<1, std::tuple_element_t<0, Gettype(same_types)>>::value;

			this->setBuffer<I>(info, hint);
		}

		template<size_t I>
		void setBuffer(RenderInfoTemplate<std::tuple_element_t<I, std::tuple<typename Buffers::value_type...>>> const& info, BufferUsageHint hint) {
			std::get<I>(this->buffers).set(info.data, hint);
		}

		void render(
			bwo::FrameBuffer& target,
			glm::ivec4 viewport,
			ogs::Configuration const& config,
			Uniforms uniforms_,
			te::optional_ref<RenderInfoTemplate<typename Buffers::value_type> const>... infos
		) {
			// Find instance count
			size_t instanceCount;
			{
				bool valid = true;
				std::optional<size_t> infoSize;

				auto zipped = te::tuple_zip(
					te::tie_tuple_elements(this->buffers),
					std::tie(infos...)
				);

				te::tuple_for_each([&](auto& tuple) {
					auto& [buffer, maybeInfo] = tuple;
					if (buffer.divisor == 0) {
						return;
					}

					if (!infoSize.has_value()) {
						infoSize = maybeInfo.value().getSize();
					}
					else if (infoSize.value() != maybeInfo.value().getSize()) {
						valid = false;
					}
					}, zipped);

				if (!infoSize.has_value()) {
					return;
				}

				instanceCount = infoSize.value();

				assert(valid);
			}
			if (instanceCount == 0) {
				return;
			}
			// End find instanc ecount

			{ // Upload buffer data
				auto zipped = te::tuple_zip(
					te::tie_tuple_elements(this->buffers),
					std::tie(infos...)
				);

				te::tuple_for_each([](auto& tuple) {
					auto& [buffer, maybeInfo] = tuple;
					if (maybeInfo.has_value()) {
						buffer.set(maybeInfo.value().data, BufferUsageHint::STREAM_DRAW);
					}
					}, zipped);
			} // End upload buffer data

			// Find amount of elements in non static buffers
			size_t elementCount;
			{
				std::optional<size_t> count;
				te::tuple_for_each([&](auto& buffer) {
					if (buffer.divisor == 0) {
						if (count.has_value()) {
							assert(count.value() == buffer.size);
						}
						else {
							count = buffer.size;
						}
					}
					}, this->buffers);

				if (!count.has_value()) {
					assert(0);
					return;
				}
				elementCount = count.value();
			}

			if (elementCount == 0) {
				return;
			}
			// End find amount of elements in non static buffers

			Global<ogs::State>->setState(config);

			auto bind = this->vao.bindScoped();
			this->program.use();

			{ // Set uniforms
				auto targetUniforms = te::get_tie(this->uniforms);
				auto storageUniforms = te::get_tie(uniforms_);

				auto zipped = te::tuple_zip(targetUniforms, storageUniforms);

				auto unit = *LazyGlobal<int, description::TexturesBound>;
				*unit = 0;

				te::tuple_for_each(
					[](auto& t) {
						auto& [target, storage] = t;
						target.setFromOther(storage);
					}, zipped
				);
			} // End set uniforms

			target.draw(
				viewport,
				[elementCount, instanceCount]() {
					glDrawArraysInstanced(
						GL_TRIANGLES,
						0,
						static_cast<GLsizei>(elementCount),
						static_cast<GLsizei>(instanceCount)
					);
				}
			);
		}

		Renderer() = delete;
		NO_COPY_MOVE(Renderer);
		~Renderer() = default;
	};

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
}
