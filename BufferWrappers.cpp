#include "BufferWrappers.h"
#include "BufferWrappers.h"
#include "BufferWrappers.h"
#include "BufferWrappers.h"
// render - A C++ OpenGL library
// Copyright (C) 2022 intrets

#include "BufferWrappers.h"

#include <fstream>
#include <sstream>

#include <mem/LazyGlobal.h>

#include <misc/PathManager.h>
#include <misc/Logger.h>

#include <render/loaders/ShaderLoader.h>
#include <render/GLStateWrapper.h>

#include <resource_data/ReadFile.h>

#include "GLStateWrapper.h"

namespace render
{
	bwo::Program::ScopedProgram bwo::Program::bind(bool resetOnDestruct) {
		return ScopedProgram(this->openglState, this->ID, resetOnDestruct);
	}

	bool bwo::Program::refreshShaders() {
		auto maybeFragBuffer = this->getFragmentBuffer();
		auto maybeVertBuffer = this->getVertexBuffer();
		if (!maybeFragBuffer || !maybeVertBuffer) {
			return false;
		}

		auto const& fragBuffer = maybeFragBuffer.value();
		auto const& vertBuffer = maybeVertBuffer.value();

		this->openglState.programRegistry.deleteProgram(*this);
		logger->logStatus("Refreshing shaders of Program {}\n", this->description);

		this->ID = LoadShaders(
			vertBuffer->getData<char>(),
			static_cast<GLint>(vertBuffer->getSize<char>()),
			fragBuffer->getData<char>(),
			static_cast<GLint>(fragBuffer->getSize<char>())
		);
		this->openglState.programRegistry.registerProgram(*this);

		return true;
	}

	bool bwo::Program::isNull() const {
		return this->ID == 0;
	}

	bool bwo::Program::isNotNull() const {
		return this->ID != 0;
	}

	bool bwo::Program::isBound() const {
		return this->openglState.isProgramBound(*this);
	}

	bwo::Program::Program(
		ogs::State& openglState_,
		char const* vert_raw, size_t vert_size,
		char const* frag_raw, size_t frag_size,
		std::string const& description_
	)
		: openglState(openglState_) {

		this->description = description_;
		logger->logStatus("Creating Program {}\n", this->description);
		this->ID = LoadShaders(vert_raw, static_cast<GLint>(vert_size), frag_raw, static_cast<GLint>(frag_size));
		this->openglState.programRegistry.registerProgram(*this);
	}

	bwo::Program::Program(
		ogs::State& openglState_,
		BufferGenerator vertexGenerator,
		BufferGenerator fragmentGenerator,
		std::string_view description_
	)
		: openglState(openglState_) {

		this->getVertexBuffer = vertexGenerator;
		this->getFragmentBuffer = fragmentGenerator;

		this->description = description_;
		logger->logStatus("Creating Program {}\n", this->description);

		[[maybe_unused]]
		bool b = this->refreshShaders();
		assert(b);
	}

	bwo::Program::Program(Program&& other) : openglState(other.openglState) {
		this->openglState.programRegistry.change(other.ID, *this);

		this->ID = other.ID;
		this->description = other.description;

		this->getFragmentBuffer = other.getFragmentBuffer;
		this->getVertexBuffer = other.getVertexBuffer;

		other.ID = 0;
		other.description = {};

		other.getFragmentBuffer = {};
		other.getVertexBuffer = {};
	}

	bwo::Program& bwo::Program::operator=(Program&& other) {
		if (this != &other) {
			if (this->ID != 0) {
				this->openglState.programRegistry.deleteProgram(*this);
			}
			this->openglState.programRegistry.change(other.ID, *this);

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
	}

	std::string_view bwo::Program::getDescription() const {
		return this->description;
	}

	bwo::Program::~Program() {
		this->openglState.programRegistry.deleteProgram(*this);
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

	bwo::FrameBuffer::FrameBuffer(ogs::State& openglState_) : openglState(openglState_) {
		glGenFramebuffers(1, &this->ID);
	}

	bwo::FrameBuffer::FrameBuffer(ogs::State& openglState_, glm::ivec2 size_) :
		ID(0),
		openglState(openglState_),
		size(size_) {
	}

	bwo::FrameBuffer::FrameBuffer(ogs::State& openglState_, int x, int y) : FrameBuffer(openglState_, glm::ivec2(x, y)) {
	}

	inline bwo::FrameBuffer::FrameBuffer(FrameBuffer&& other) : openglState(other.openglState) {
		this->ID = other.ID;
		this->size = other.size;

		other.ID = 0;
		other.size = { 0,0 };
	}

	bwo::FrameBuffer& bwo::FrameBuffer::operator=(FrameBuffer&& other) {
		assert(this->openglState == other.openglState);

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

	bwo::FrameBuffer::~FrameBuffer() {
		glDeleteFramebuffers(1, &this->ID);
	}

	static auto getAttachmentEnum(int32_t number) {
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
		assert(this->openglState.MAX_COLOR_ATTACHMENTS > attachmentNumber);

		this->size = texture.size;
		this->openglState.setFrameBuffer(this->ID);
		glFramebufferTexture(GL_FRAMEBUFFER, getAttachmentEnum(attachmentNumber), texture.ID, mipmap);
	}

	void bwo::FrameBuffer::bindTextureColorLayer(int32_t attachmentNumber, bwo::Texture2DArray const& texture, int32_t mipmap, int32_t layer) {
		this->size.x = texture.size.x;
		this->size.y = texture.size.y;

		this->openglState.setFrameBuffer(this->ID);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, getAttachmentEnum(attachmentNumber), texture.ID, mipmap, layer);
	}

	void bwo::FrameBuffer::bindDepthLayer(bwo::Texture2D const& texture) {
		this->size.x = texture.size.x;
		this->size.y = texture.size.y;

		this->openglState.setFrameBuffer(this->ID);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.ID, 0);
	}

	void bwo::FrameBuffer::draw(glm::ivec4 viewport, std::function<void()> f) {
		this->openglState.setFrameBuffer(this->ID);
		this->openglState.setViewport(viewport);

		f();
	}

	void bwo::FrameBuffer::clear(glm::vec4 color, bool depth) {
		this->openglState.setFrameBuffer(this->ID);
		glClearColor(color.r, color.g, color.b, color.a);
		if (depth) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}

	void bwo::FrameBuffer::clearDepth() {
		this->openglState.setFrameBuffer(this->ID);
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

		int32_t maxSize = 0;
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

	bwo::Texture2D bwo::Texture2DHelper::makeFloatBuffer(glm::ivec2 size, bool filtering, bool mip) {
		GLenum filterModeMag = filtering ? GL_LINEAR : GL_NEAREST;
		GLenum filterModeMin = filtering ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
		Texture2D res{
			size,
			0,
			GL_R32F,
			filterModeMag,
			filterModeMin,
			GL_REPEAT,
			GL_REPEAT,
			0,
			GL_RED,
			GL_FLOAT,
			(void*)0
		};
		return res;
	}

	bwo::Texture2D bwo::Texture2DHelper::makeVec2Buffer(glm::ivec2 size, bool filtering) {
		GLenum filterMode = filtering ? GL_LINEAR : GL_NEAREST;
		Texture2D res{
			size,
			0,
			GL_RG32F,
			filterMode,
			filterMode,
			GL_REPEAT,
			GL_REPEAT,
			0,
			GL_RG,
			GL_FLOAT,
			(void*)0
		};
		return res;
	}

	bwo::Texture2D bwo::Texture2DHelper::makeVec3Buffer(glm::ivec2 size, bool filtering) {
		GLenum filterMode = filtering ? GL_LINEAR : GL_NEAREST;
		Texture2D res{
			size,
			0,
			GL_RGB32F,
			filterMode,
			filterMode,
			GL_REPEAT,
			GL_REPEAT,
			0,
			GL_RGB,
			GL_FLOAT,
			(void*)0
		};
		return res;
	}

	bwo::Texture2D bwo::Texture2DHelper::makeVec4Buffer(glm::ivec2 size, bool filtering) {
		GLenum filterMode = filtering ? GL_LINEAR : GL_NEAREST;
		Texture2D res{
			size,
			0,
			GL_RGBA32F,
			filterMode,
			filterMode,
			GL_REPEAT,
			GL_REPEAT,
			0,
			GL_RGBA,
			GL_FLOAT,
			(void*)0
		};
		return res;
	}

	bwo::Texture2D bwo::Texture2DHelper::makeHDRBuffer(glm::ivec2 size) {
		return Texture2D(
			size,
			0,
			GL_RGBA16F,
			GL_LINEAR,
			GL_LINEAR,
			GL_REPEAT,
			GL_REPEAT,
			0,
			GL_RGBA,
			GL_FLOAT,
			(void*)0
		);
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

	bwo::Texture2D bwo::Texture2DHelper::makeAccumulateBuffer(glm::ivec2 size) {
		Texture2D res{
			size,
			0,
			GL_RGBA32F,
			GL_LINEAR,
			GL_LINEAR,
			GL_CLAMP,
			GL_CLAMP,
			0,
			GL_RGB,
			GL_FLOAT,
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
		Program::ScopedProgram::ScopedProgram(ogs::State& openglState_, GLint id, bool resetOnDestruct_) : resetOnDestruct(resetOnDestruct_), openglState(openglState_) {
			this->openglState.setProgram(id);
		}

		Program::ScopedProgram::~ScopedProgram() {
			if (this->resetOnDestruct) {
				this->openglState.setProgram(0);
			}
		}
	}
}

