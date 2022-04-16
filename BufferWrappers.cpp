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

#include "BufferWrappers.h"

#include <fstream>
#include <sstream>

#include <mem/LazyGlobal.h>

#include <misc/PathManager.h>
#include <misc/Logger.h>

#include <render/loaders/ShaderLoader.h>

#include <resource_data/ReadFile.h>

#include "GLStateWrapper.h"

namespace render
{
	std::unordered_map<int32_t, bwo::Program*> bwo::Program::refs;

	std::string bwo::Program::listAll() {
		std::stringstream out;

		for (auto& [index, program] : bwo::Program::refs) {
			out << "ID: " << index << '\n' << program->description << "\n\n";
		}

		return out.str();
	}

	std::optional<bwo::Program const*> bwo::Program::lookup(int32_t id) {
		auto it = bwo::Program::refs.find(id);

		if (it == bwo::Program::refs.end()) {
			return std::nullopt;
		}
		else {
			return it->second;
		}
	}

	bwo::Program::ScopedProgram bwo::Program::bind(bool resetOnDestruct) {
		return ScopedProgram(this->ID, resetOnDestruct);
	}

	bool bwo::Program::refreshAll() {
		std::vector<Program*> programs;
		for (auto [i, program] : refs) {
			programs.push_back(program);
		}

		bool success = true;
		for (auto program : programs) {
			if (!program->refreshShaders()) {
				success = false;
			}
		}

		return success;
	}

	void bwo::Program::addProgram(Program& program) {
		assert(program.ID != 0);
		assert(!lookup(program.ID).has_value());
		refs[program.ID] = &program;
	}

	void bwo::Program::change(GLuint ID, Program& to) {
		if (ID == 0) {
			return;
		}
		assert(lookup(ID).has_value());
		refs[ID] = &to;
	}

	void bwo::Program::deleteProgram(Program& program) {
		if (program.ID == 0) {
			return;
		}
		assert(lookup(program.ID).has_value());
		refs.erase(program.ID);
		glDeleteProgram(program.ID);
	}

	bool bwo::Program::refreshShaders() {
		auto maybeFragBuffer = this->getFragmentBuffer();
		auto maybeVertBuffer = this->getVertexBuffer();
		if (!maybeFragBuffer || !maybeVertBuffer) {
			return false;
		}

		auto const& fragBuffer = maybeFragBuffer.value();
		auto const& vertBuffer = maybeVertBuffer.value();

		deleteProgram(*this);
		logger->acquire()->log(Logger::Level::status, "Refreshing shaders of Program {}\n", this->description);

		this->ID = LoadShaders(
			vertBuffer->getData<char>(),
			static_cast<GLint>(vertBuffer->getSize<char>()),
			fragBuffer->getData<char>(),
			static_cast<GLint>(fragBuffer->getSize<char>())
		);
		addProgram(*this);

		return true;
	}

	bwo::Program::Program(
		char const* vert_raw, size_t vert_size,
		char const* frag_raw, size_t frag_size,
		std::string const& description_
	) {
		this->description = description_;
		logger->acquire()->log(Logger::Level::status, "Creating Program {}\n", this->description);
		this->ID = LoadShaders(vert_raw, static_cast<GLint>(vert_size), frag_raw, static_cast<GLint>(frag_size));
		addProgram(*this);
	}

	bwo::Program::Program(BufferGenerator vertexGenerator, BufferGenerator fragmentGenerator, std::string_view description_) {
		this->getVertexBuffer = vertexGenerator;
		this->getFragmentBuffer = fragmentGenerator;

		this->description = description_;
		logger->acquire()->log(Logger::Level::status, "Creating Program {}\n", this->description);

		[[maybe_unused]]
		bool b = this->refreshShaders();
		assert(b);
	}

	std::string_view bwo::Program::getDescription() const {
		return this->description;
	}

	bwo::Program::~Program() {
		deleteProgram(*this);
	}

	bwo::UniformMatrix4fv::UniformMatrix4fv(std::string name, Program const& program) {
		this->location = glGetUniformLocation(program.ID, name.c_str());
	}

	void bwo::UniformMatrix4fv::set(glm::mat4 const& mat) {
		glUniformMatrix4fv(this->location, 1, GL_FALSE, &mat[0][0]);
	}

	void bwo::UniformTexture2D::set(GLuint texture) {
		glActiveTexture(GL_TEXTURE0 + this->unit);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(this->location, this->unit);
	}

	void bwo::UniformTexture2D::set(bwo::Texture2D const& texture) {
		this->set(texture.ID);
	}

	bwo::UniformTexture2D::UniformTexture2D(std::string name, Program const& program, int32_t unit_) {
		this->unit = unit_;
		this->location = glGetUniformLocation(program.ID, name.c_str());
	}

	void bwo::Uniform3fv::set(glm::vec3 vec) {
		glUniform3fv(this->location, 1, &vec[0]);
	}

	bwo::Uniform3fv::Uniform3fv(std::string name, Program const& program) {
		this->location = glGetUniformLocation(program.ID, name.c_str());
	}

	void bwo::Uniform1f::set(float f) {
		glUniform1f(this->location, f);
	}

	bwo::Uniform1f::Uniform1f(std::string name, Program const& program) {
		this->location = glGetUniformLocation(program.ID, name.c_str());
	}

	void bwo::UniformTexture2DArray::set(Texture2DArray const& texture) {
		glActiveTexture(GL_TEXTURE0 + this->unit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture.ID);
		glUniform1i(location, this->unit);
	}

	bwo::UniformTexture2DArray::UniformTexture2DArray(std::string name, Program const& program, int32_t unit_) {
		this->unit = unit_;
		this->location = glGetUniformLocation(program.ID, name.c_str());
	}

	void bwo::Uniform1i::set(int32_t i) {
		glUniform1i(this->location, i);
	}

	bwo::Uniform1i::Uniform1i(std::string name, Program const& program) {
		this->location = glGetUniformLocation(program.ID, name.c_str());
	}

	void bwo::Uniform2fv::set(glm::vec2 vec) {
		glUniform2fv(this->location, 1, &vec[0]);
	}

	bwo::Uniform2fv::Uniform2fv(std::string name, Program const& program) {
		this->location = glGetUniformLocation(program.ID, name.c_str());
	}

	void bwo::Uniform2iv::set(glm::ivec2 vec) {
		glUniform2iv(this->location, 1, &vec[0]);
	}

	void bwo::Uniform2iv::set(std::vector<glm::ivec2> const& vecs) {
		glUniform2iv(this->location, static_cast<GLsizei>(vecs.size()), &vecs[0][0]);
	}

	bwo::Uniform2iv::Uniform2iv(std::string name, Program const& program) {
		this->location = glGetUniformLocation(program.ID, name.c_str());
	}

	void bwo::Uniform4fv::set(glm::vec4 vec) {
		glUniform4fv(this->location, 1, &vec[0]);
	}

	bwo::Uniform4fv::Uniform4fv(std::string name, Program const& program) {
		this->location = glGetUniformLocation(program.ID, name.c_str());
	}

	bwo::FrameBuffer::FrameBuffer() {
		glGenFramebuffers(1, &this->ID);
	}

	bwo::FrameBuffer::FrameBuffer(int x, int y) {
		this->ID = 0;
		this->size.x = x;
		this->size.y = y;
	}

	bwo::FrameBuffer::~FrameBuffer() {
		glDeleteFramebuffers(1, &this->ID);
	}

	static auto getAttachmentEnum(int32_t number) {
		assert(LazyGlobal<ogs::State>->MAX_COLOR_ATTACHMENTS > number);

		GLenum attachment = GL_COLOR_ATTACHMENT0;
		switch (number) {
			case 0:
				attachment = GL_COLOR_ATTACHMENT0;
				break;
			case 1:
				attachment = GL_COLOR_ATTACHMENT1;
				break;
			case 2:
				attachment = GL_COLOR_ATTACHMENT2;
				break;
			case 3:
				attachment = GL_COLOR_ATTACHMENT3;
				break;
			case 4:
				attachment = GL_COLOR_ATTACHMENT4;
				break;
			case 5:
				attachment = GL_COLOR_ATTACHMENT5;
				break;
			case 6:
				attachment = GL_COLOR_ATTACHMENT6;
				break;
			case 7:
				attachment = GL_COLOR_ATTACHMENT7;
				break;
			default:
				assert(0);
				break;
		}
		return attachment;
	}

	void bwo::FrameBuffer::bindTextureColor(int32_t attachmentNumber, bwo::Texture2D const& texture, GLint mipmap) {
		assert(LazyGlobal<ogs::State>->MAX_COLOR_ATTACHMENTS > attachmentNumber);

		this->size = texture.size;
		LazyGlobal<ogs::State>->setFrameBuffer(this->ID);
		glFramebufferTexture(GL_FRAMEBUFFER, getAttachmentEnum(attachmentNumber), texture.ID, mipmap);
	}

	void bwo::FrameBuffer::bindTextureColorLayer(int32_t attachmentNumber, bwo::Texture2DArray const& texture, int32_t mipmap, int32_t layer) {
		this->size.x = texture.size.x;
		this->size.y = texture.size.y;

		LazyGlobal<ogs::State>->setFrameBuffer(this->ID);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, getAttachmentEnum(attachmentNumber), texture.ID, mipmap, layer);
	}

	void bwo::FrameBuffer::bindDepthLayer(bwo::Texture2D const& texture) {
		this->size.x = texture.size.x;
		this->size.y = texture.size.y;

		LazyGlobal<ogs::State>->setFrameBuffer(this->ID);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.ID, 0);
	}

	void bwo::FrameBuffer::draw(glm::ivec4 viewport, std::function<void()> f) {
		LazyGlobal<ogs::State>->setFrameBuffer(this->ID);
		LazyGlobal<ogs::State>->setViewport(viewport);

		f();
	}

	void bwo::FrameBuffer::clear(glm::vec4 color, bool depth) {
		LazyGlobal<ogs::State>->setFrameBuffer(this->ID);
		glClearColor(color.r, color.g, color.b, color.a);
		if (depth) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}

	void bwo::FrameBuffer::clearDepth() {
		LazyGlobal<ogs::State>->setFrameBuffer(this->ID);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void bwo::FrameBuffer::onlyDepth() {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	bwo::Texture2D::Texture2D() {
		this->ID = 0;
		this->size = { 0,0 };
	}

	bwo::Texture2D::Texture2D(GLuint handle) {
		this->size = glm::ivec2(0, 0);
		this->ID = handle;
	}

	bwo::Texture2D::Texture2D(glm::ivec2 size_, GLint level, GLenum internalFormat, GLenum magFilter, GLenum minFilter, GLenum wrapS, GLenum wrapT, GLint border, GLenum pixelFormat, GLenum type, void const* pixels) {
		glGenTextures(1, &this->ID);
		glBindTexture(GL_TEXTURE_2D, this->ID);

		int32_t maxSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

		this->size = glm::min(glm::ivec2(maxSize), size_);

		glTexImage2D(GL_TEXTURE_2D, level, internalFormat, this->size.x, this->size.y, border, pixelFormat, type, pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	}

	bwo::Texture2D::~Texture2D() {
		glDeleteTextures(1, &this->ID);
	}

	bwo::Texture2D::Texture2D(Texture2D&& other) {
		this->ID = other.ID;
		other.ID = 0;

		this->size = other.size;
	}

	bwo::Texture2D& bwo::Texture2D::operator=(Texture2D&& other) {
		if (this == &other) {
			return *this;
		}

		glDeleteTextures(1, &this->ID);
		this->ID = other.ID;
		other.ID = 0;

		this->size = other.size;

		return *this;
	}

	bwo::Texture2D bwo::Texture2DHelper::makeLinearFiltering(glm::ivec2 size) {
		Texture2D res{
			size,
			0,
			GL_RGBA,
			GL_LINEAR,
			GL_LINEAR,
			GL_REPEAT,
			GL_REPEAT,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			(void*)0
		};
		return res;
	}

	bwo::Texture2D bwo::Texture2DHelper::makeFloatBuffer(glm::ivec2 size, bool filtering) {
		GLenum filterMode = filtering ? GL_LINEAR : GL_NEAREST;
		Texture2D res{
			size,
			0,
			GL_R32F,
			filterMode,
			filterMode,
			GL_REPEAT,
			GL_REPEAT,
			0,
			GL_RED,
			GL_FLOAT,
			(void*)0
		};
		return res;
	}

	bwo::Texture2D bwo::Texture2DHelper::makeNoFiltering(glm::ivec2 size) {
		Texture2D res{
			size,
			0,
			GL_RGBA,
			GL_NEAREST,
			GL_NEAREST,
			GL_REPEAT,
			GL_REPEAT,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			(void*)0
		};
		return res;
	}

	bwo::Texture2DArray::Texture2DArray(glm::ivec3 size_, GLint level, GLenum internalFormat, GLenum magFilter, GLenum minFilter, GLenum wrapS, GLenum wrapT, GLint border, GLenum pixelFormat, GLenum type, void const* pixels) {
		glGenTextures(1, &this->ID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, this->ID);

		int32_t maxSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

		int32_t maxLayers;
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxLayers);

		this->size = glm::min(glm::ivec3(maxSize, maxSize, maxLayers), size_);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, level, internalFormat, size.x, size.y, size.z, border, pixelFormat, type, pixels);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrapT);
	}

	bwo::Texture2DArray::~Texture2DArray() {
		glDeleteTextures(1, &this->ID);
	}

	bwo::Texture2DArray::Texture2DArray(Texture2DArray&& other) {
		this->ID = other.ID;
		other.ID = 0;

		this->size = other.size;
	}

	bwo::Texture2DArray& bwo::Texture2DArray::operator=(Texture2DArray&& other) {
		if (this == &other) {
			return *this;
		}

		glDeleteTextures(1, &this->ID);
		this->ID = other.ID;
		other.ID = 0;

		this->size = other.size;

		return *this;
	}

	bwo::Texture2DArray bwo::Texture2DArrayHelper::makeLinearFiltering(glm::ivec3 size) {
		Texture2DArray res{
			size,
			0,
			GL_RGBA,
			GL_LINEAR,
			GL_LINEAR,
			GL_REPEAT,
			GL_REPEAT,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			(void*)0
		};
		return res;
	}

	bwo::Texture2D bwo::Texture2DHelper::makeDepthBuffer(glm::ivec2 size) {
		Texture2D res{
			size,
			0,
			GL_DEPTH_COMPONENT32F,
			GL_NEAREST,
			GL_NEAREST,
			GL_CLAMP_TO_EDGE,
			GL_CLAMP_TO_EDGE,
			0,
			GL_DEPTH_COMPONENT,
			GL_FLOAT,
			(void*)0
		};
		return res;
	}

	namespace bwo
	{
		Program::ScopedProgram::ScopedProgram(GLint id, bool resetOnDestruct_) : resetOnDestruct(resetOnDestruct_) {
			LazyGlobal<ogs::State>->setProgram(id);
		}

		Program::ScopedProgram::~ScopedProgram() {
			if (this->resetOnDestruct) {
				LazyGlobal<ogs::State>->setProgram(0);
			}
		}
	}
}

