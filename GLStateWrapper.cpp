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

#include <wrangled_gl/wrangled_gl.h>

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

	bool State::isProgramBound(GLint id) const {
		return this->program.has_value() && this->program == id;
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
			BLEND::ENABLED,
			BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
			DEPTH_TEST::ENABLED,
			DEPTH_FUNC::LESS,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration UIBackground() {
		return {
			BLEND::DISABLED,
			BLEND_FUNC::UNSET,
			DEPTH_TEST::ENABLED,
			DEPTH_FUNC::LESS,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration FontAtlasConfiguration() {
		return {
			BLEND::DISABLED,
			BLEND_FUNC::UNSET,
			DEPTH_TEST::DISABLED,
			DEPTH_FUNC::UNSET,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration WorldTileConfiguration() {
		return {
			BLEND::DISABLED,
			BLEND_FUNC::UNSET,
			DEPTH_TEST::DISABLED,
			DEPTH_FUNC::UNSET,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration BlockIDConfiguration() {
		return {
			BLEND::DISABLED,
			BLEND_FUNC::UNSET,
			DEPTH_TEST::DISABLED,
			DEPTH_FUNC::UNSET,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration DebugPointConfiguration() {
		return {
			BLEND::DISABLED,
			BLEND_FUNC::UNSET,
			DEPTH_TEST::DISABLED,
			DEPTH_FUNC::UNSET,
			POLYGON_MODE::POINT,
			10.0f,
		};
	}

	Configuration DebugLineConfiguration() {
		return {
			BLEND::DISABLED,
			BLEND_FUNC::UNSET,
			DEPTH_TEST::DISABLED,
			DEPTH_FUNC::UNSET,
			POLYGON_MODE::LINE,
			0.0f,
		};
	}

	Configuration NinesConfiguration() {
		return {
			BLEND::ENABLED,
			BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
			DEPTH_TEST::ENABLED,
			DEPTH_FUNC::LEQUAL,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration DefaultConfiguration() {
		return {
			BLEND::ENABLED,
			BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
			DEPTH_TEST::DISABLED,
			DEPTH_FUNC::UNSET,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration HighlightConfiguration() {
		return {
			BLEND::ENABLED,
			BLEND_FUNC::SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
			DEPTH_TEST::DISABLED,
			DEPTH_FUNC::UNSET,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration GeneralConfiguration() {
		return {
			BLEND::DISABLED,
			BLEND_FUNC::UNSET,
			DEPTH_TEST::ENABLED,
			DEPTH_FUNC::LESS,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration ShadowMapConfiguration() {
		return {
			BLEND::DISABLED,
			BLEND_FUNC::UNSET,
			DEPTH_TEST::ENABLED,
			DEPTH_FUNC::LESS,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration BlitConfiguration() {
		return {
			BLEND::DISABLED,
			BLEND_FUNC::UNSET,
			DEPTH_TEST::DISABLED,
			DEPTH_FUNC::UNSET,
			POLYGON_MODE::FILL,
			0.0f,
		};
	}

	Configuration::Configuration(BLEND blend_, BLEND_FUNC blendFunc_, DEPTH_TEST depthTest_, DEPTH_FUNC depthFunc_, POLYGON_MODE polygonMode_, float pointSize_) :
		blend(blend_),
		blendFunc(blendFunc_),
		depthTest(depthTest_),
		depthFunc(depthFunc_),
		polygonMode(polygonMode_),
		pointSize(pointSize_) {
	}
}
