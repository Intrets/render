#include "GLStateWrapper.h"
#include "GLStateWrapper.h"
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

#include "GLStateWrapper.h"

#include "BufferWrappers.h"

#include <cassert>

namespace ogs
{
	State::State() {
		this->setBlend(BLEND::ENABLED);
		this->setBlendFunc(BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA);
		this->setDepthTest(DEPTH_TEST::ENABLED);
		this->setDepthFunc(DEPTH_FUNC::LESS);
		this->setPolygonMode(POLYGON_MODE::FILL);

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &this->MAX_COLOR_ATTACHMENTS);
	}

	void State::setConfiguration(Configuration const& config) {
		this->setDepthMask(config.depthMask);

		this->setBlend(config.blend);
		if (config.blend == BLEND::ENABLED) {
			this->setBlendFunc(config.blendFunc);
		}

		this->setDepthTest(config.depthTest);
		if (config.depthTest == DEPTH_TEST::ENABLED) {
			this->setDepthFunc(config.depthFunc);
		}

		this->setPolygonMode(config.polygonMode);
	}

	void State::setBlend(BLEND b) {
		if (this->configuration.blend != b) {
			this->configuration.blend = b;
			switch (b) {
				case BLEND::ENABLED:
					glEnable(GL_BLEND);
					break;
				case BLEND::DISABLED:
					glDisable(GL_BLEND);
					break;
				default:
					assert(0);
					break;
			}

		}
	}

	void State::setBlendFunc(BLEND_FUNC func) {
		if (this->configuration.blendFunc != func) {
			this->configuration.blendFunc = func;
			switch (func) {
				case BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case BLEND_FUNC::SRC_ONE__ONE_MINUS_SRC_ALPHA:
					glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case BLEND_FUNC::ONE__ZERO:
					glBlendFunc(GL_ONE, GL_ZERO);
					break;
				case BLEND_FUNC::ZERO__ONE:
					glBlendFunc(GL_ZERO, GL_ONE);
					break;
				case BLEND_FUNC::ONE__ONE:
					glBlendFunc(GL_ONE, GL_ONE);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void State::setDepthTest(DEPTH_TEST test) {
		if (this->configuration.depthTest != test) {
			this->configuration.depthTest = test;
			switch (test) {
				case DEPTH_TEST::ENABLED:
					glEnable(GL_DEPTH_TEST);
					break;
				case DEPTH_TEST::DISABLED:
					glDisable(GL_DEPTH_TEST);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void State::setDepthFunc(DEPTH_FUNC func) {
		if (this->configuration.depthFunc != func) {
			this->configuration.depthFunc = func;
			switch (func) {
				case DEPTH_FUNC::LESS:
					glDepthFunc(GL_LESS);
					break;
				case DEPTH_FUNC::LEQUAL:
					glDepthFunc(GL_LEQUAL);
					break;
				case DEPTH_FUNC::ALWAYS:
					glDepthFunc(GL_ALWAYS);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void ogs::State::setPolygonMode(POLYGON_MODE mode) {
#define GL_PROGRAM_POINT_SIZE 0x8642
		if (this->configuration.polygonMode != mode) {
			if (this->configuration.polygonMode == POLYGON_MODE::POINT) {
				glDisable(GL_PROGRAM_POINT_SIZE);
			}

			this->configuration.polygonMode = mode;
			switch (mode) {
				case POLYGON_MODE::FILL:
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					break;
				case POLYGON_MODE::LINE:
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					break;
				case POLYGON_MODE::POINT:
					glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
					glEnable(GL_PROGRAM_POINT_SIZE);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void State::setDepthMask(DEPTH_MASK b) {
		if (this->configuration.depthMask != b) {
			this->configuration.depthMask = b;
			switch (b) {
				case DEPTH_MASK::FALSE:
					glDepthMask(GL_FALSE);
					break;
				case DEPTH_MASK::TRUE:
					glDepthMask(GL_TRUE);
					break;
				default:
					assert(0);
					break;
			}
		}
	}

	void State::setViewport(glm::ivec4 p) {
		if (this->viewport != p) {
			this->viewport = p;

			glViewport(
				static_cast<GLint>(this->viewport->x),
				static_cast<GLint>(this->viewport->y),
				static_cast<GLsizei>(this->viewport->z),
				static_cast<GLsizei>(this->viewport->w)
			);
		}
	}

	void State::setFrameBuffer(GLuint id) {
		if (this->frameBuffer != id) {
			this->frameBuffer = id;

			glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer.value());
		}
	}

	void State::setFrameBuffer(render::bwo::FrameBuffer const& frameBuffer) {
		this->setFrameBuffer(frameBuffer.ID);
	}

	void State::setVAO(GLint id) {
#ifdef DEBUG_BUILD
		if (this->vao.has_value()) {
			GLint p = 0;
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &p);
			assert(p == this->vao.value());
		}
#endif

		if (this->vao != id) {
			this->vao = id;

			glBindVertexArray(this->vao.value());
		}
	}

	void State::setProgram(GLint id) {
#ifdef DEBUG_BUILD
		if (this->program.has_value()) {
			GLint p = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &p);
			assert(p == this->program.value());
		}
#endif
		if (this->program != id) {
			this->program = id;

			glUseProgram(this->program.value());
		}
	}

	bool State::isProgramBound(render::bwo::Program const& p) const {
		return this->program.has_value() && this->program == p.ID;
	}

	bool State::isVAOBound(GLint id) const {
		return this->vao.has_value() && this->vao == id;
	}

	void State::flushState() {
		this->configuration.blend = BLEND::UNSET;
		this->configuration.blendFunc = BLEND_FUNC::UNSET;
		this->configuration.depthTest = DEPTH_TEST::UNSET;
		this->configuration.depthFunc = DEPTH_FUNC::UNSET;
		this->configuration.polygonMode = POLYGON_MODE::UNSET;

		this->viewport = std::nullopt;
		this->frameBuffer = std::nullopt;
		this->vao = std::nullopt;
		this->program = std::nullopt;
	}

	Configuration TextConfiguration() {
		return {
			.blend = BLEND::ENABLED,
			.blendFunc = BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
			.depthTest = DEPTH_TEST::ENABLED,
			.depthFunc = DEPTH_FUNC::LESS,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration UIBackground() {
		return {
			.blend = BLEND::DISABLED,
			.blendFunc = BLEND_FUNC::UNSET,
			.depthTest = DEPTH_TEST::ENABLED,
			.depthFunc = DEPTH_FUNC::LESS,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration FontAtlasConfiguration() {
		return {
			.blend = BLEND::DISABLED,
			.blendFunc = BLEND_FUNC::UNSET,
			.depthTest = DEPTH_TEST::DISABLED,
			.depthFunc = DEPTH_FUNC::UNSET,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration WorldTileConfiguration() {
		return {
			.blend = BLEND::DISABLED,
			.blendFunc = BLEND_FUNC::UNSET,
			.depthTest = DEPTH_TEST::DISABLED,
			.depthFunc = DEPTH_FUNC::UNSET,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration BlockIDConfiguration() {
		return {
			.blend = BLEND::DISABLED,
			.blendFunc = BLEND_FUNC::UNSET,
			.depthTest = DEPTH_TEST::DISABLED,
			.depthFunc = DEPTH_FUNC::UNSET,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration DebugPointConfiguration() {
		return {
			.blend = BLEND::DISABLED,
			.blendFunc = BLEND_FUNC::UNSET,
			.depthTest = DEPTH_TEST::DISABLED,
			.depthFunc = DEPTH_FUNC::UNSET,
			.polygonMode = POLYGON_MODE::POINT,
			.pointSize = 10.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration DebugLineConfiguration() {
		return {
			.blend = BLEND::DISABLED,
			.blendFunc = BLEND_FUNC::UNSET,
			.depthTest = DEPTH_TEST::DISABLED,
			.depthFunc = DEPTH_FUNC::UNSET,
			.polygonMode = POLYGON_MODE::LINE,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration NinesConfiguration() {
		return {
			.blend = BLEND::ENABLED,
			.blendFunc = BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
			.depthTest = DEPTH_TEST::ENABLED,
			.depthFunc = DEPTH_FUNC::LEQUAL,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration ParticleConfiguration() {
		return {
			.blend = BLEND::ENABLED,
			.blendFunc = BLEND_FUNC::SRC_ONE__ONE_MINUS_SRC_ALPHA,
			.depthTest = DEPTH_TEST::ENABLED,
			.depthFunc = DEPTH_FUNC::LEQUAL,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::FALSE
		};
	}

	Configuration BadBloomConfiguration() {
		return {
			.blend = BLEND::ENABLED,
			.blendFunc = BLEND_FUNC::ONE__ONE,
			.depthTest = DEPTH_TEST::DISABLED,
			.depthFunc = DEPTH_FUNC::UNSET,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::FALSE,
		};
	}

	Configuration DefaultConfiguration() {
		return {
			.blend = BLEND::ENABLED,
			.blendFunc = BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
			.depthTest = DEPTH_TEST::DISABLED,
			.depthFunc = DEPTH_FUNC::UNSET,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration HighlightConfiguration() {
		return {
			.blend = BLEND::ENABLED,
			.blendFunc = BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
			.depthTest = DEPTH_TEST::DISABLED,
			.depthFunc = DEPTH_FUNC::UNSET,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration GeneralConfiguration() {
		return {
			.blend = BLEND::DISABLED,
			.blendFunc = BLEND_FUNC::UNSET,
			.depthTest = DEPTH_TEST::ENABLED,
			.depthFunc = DEPTH_FUNC::LESS,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration ShadowMapConfiguration() {
		return {
			.blend = BLEND::DISABLED,
			.blendFunc = BLEND_FUNC::UNSET,
			.depthTest = DEPTH_TEST::ENABLED,
			.depthFunc = DEPTH_FUNC::LESS,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::TRUE,
		};
	}

	Configuration BlitConfiguration() {
		return {
			.blend = BLEND::DISABLED,
			.blendFunc = BLEND_FUNC::UNSET,
			.depthTest = DEPTH_TEST::DISABLED,
			.depthFunc = DEPTH_FUNC::UNSET,
			.polygonMode = POLYGON_MODE::FILL,
			.pointSize = 0.0f,
			.depthMask = DEPTH_MASK::FALSE,
		};
	}

	std::string ProgramRegistry::listAll() {
		std::stringstream out;

		for (auto& [index, program] : this->programs) {
			out << "ID: " << index << '\n' << program->getDescription() << "\n\n";
		}

		return out.str();
	}

	te::optional_ref<render::bwo::Program> ProgramRegistry::lookup(GLint ID) {
		auto it = this->programs.find(ID);

		if (it == this->programs.end()) {
			return te::nullopt;
		}
		else {
			return *it->second;
		}
	}

	te::optional_ref<render::bwo::Program> ProgramRegistry::lookup(render::bwo::Program const& program) {
		return this->lookup(program.ID);
	}

	bool ProgramRegistry::refreshAll() {
		std::vector<render::bwo::Program*> refreshPrograms;
		for (auto [i, program] : this->programs) {
			refreshPrograms.push_back(program);
		}

		bool success = true;
		for (auto program : refreshPrograms) {
			if (!program->refreshShaders()) {
				success = false;
			}
		}

		return success;
	}

	void ProgramRegistry::change(GLint ID, render::bwo::Program& to) {
		if (ID == 0) {
			return;
		}

		assert(this->lookup(ID).has_value());

		this->programs[ID] = &to;
	}

	void ProgramRegistry::registerProgram(render::bwo::Program& program) {
		assert(program.isNotNull());
		assert(this->lookup(program).has_no_value());

		this->programs[program.ID] = &program;
	}

	void ProgramRegistry::deleteProgram(render::bwo::Program& program) {
		if (program.isNotNull()) {
			assert(this->lookup(program).has_value());

			this->programs.erase(program.ID);
			glDeleteProgram(program.ID);
		}
	}

	bool ProgramRegistry::operator==(ProgramRegistry const& other) const {
		return this == &other;
	}

	bool State::operator==(State const& other) const {
		return this == &other;
	}
}
