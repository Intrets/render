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

#include <wglm/vec4.hpp>

#include <tepp/optional_ref.h>

#include <misc/Misc.h>

#include <cstdint>
#include <optional>
#include <unordered_map>

enum class BLEND
{
	UNSET,
	ENABLED,
	DISABLED,
	MAX,
};

enum class BLEND_FUNC
{
	UNSET,
	SRC_ALPHA__ONE_MINUS_SRC_ALPHA,
	SRC_ONE__ONE_MINUS_SRC_ALPHA,
	ONE__ZERO,
	ZERO__ONE,
	MAX,
};

enum class DEPTH_TEST
{
	UNSET,
	ENABLED,
	DISABLED,
	MAX,
};

enum class DEPTH_FUNC
{
	UNSET,
	LESS,
	LEQUAL,
	MAX,
};

enum class POLYGON_MODE
{
	UNSET,
	FILL,
	LINE,
	POINT,
	MAX,
};

namespace render::bwo
{
	class Program;
}

namespace ogs
{
	class State;

	class Configuration
	{
	public:
		BLEND blend = BLEND::UNSET;
		BLEND_FUNC blendFunc = BLEND_FUNC::UNSET;
		DEPTH_TEST depthTest = DEPTH_TEST::UNSET;
		DEPTH_FUNC depthFunc = DEPTH_FUNC::UNSET;
		POLYGON_MODE polygonMode = POLYGON_MODE::UNSET;
		float pointSize = 0.0f;

		Configuration(
			BLEND blend,
			BLEND_FUNC blendFunc,
			DEPTH_TEST depthTest,
			DEPTH_FUNC depthFunc,
			POLYGON_MODE polygonMode,
			float pointSize
		);

	private:
		Configuration() = default;

		friend class State;
	};

	Configuration TextConfiguration();
	Configuration UIBackground();
	Configuration FontAtlasConfiguration();
	Configuration WorldTileConfiguration();
	Configuration BlockIDConfiguration();
	Configuration DebugPointConfiguration();
	Configuration DebugLineConfiguration();
	Configuration DefaultConfiguration();
	Configuration HighlightConfiguration();
	Configuration GeneralConfiguration();
	Configuration ShadowMapConfiguration();
	Configuration BlitConfiguration();
	Configuration NinesConfiguration();

	class ProgramRegistry
	{
	private:
		std::unordered_map<GLint, render::bwo::Program*> programs;

	public:
		std::string listAll();
		te::optional_ref<render::bwo::Program> lookup(GLint ID);
		te::optional_ref<render::bwo::Program> lookup(render::bwo::Program const& program);

		[[nodiscard]]
		bool refreshAll();

		void change(GLint ID, render::bwo::Program& to);
		void registerProgram(render::bwo::Program& program);
		void deleteProgram(render::bwo::Program& program);

		bool operator==(ProgramRegistry const& other) const;

		ProgramRegistry() = default;
		~ProgramRegistry() = default;

		NO_COPY_MOVE(ProgramRegistry);
	};

	class State
	{
	public:
		ProgramRegistry programRegistry{};
		Configuration configuration{};

		std::optional<glm::ivec4> viewport{};
		std::optional<GLuint> frameBuffer{};
		std::optional<GLint> vao{};
		std::optional<GLint> program{};

		int32_t MAX_COLOR_ATTACHMENTS;

		void setConfiguration(Configuration const& config);

		void setBlend(BLEND b);
		void setBlendFunc(BLEND_FUNC func);
		void setDepthTest(DEPTH_TEST test);
		void setDepthFunc(DEPTH_FUNC func);
		void setPolygonMode(POLYGON_MODE mode);

		void setViewport(glm::ivec4 p);
		void setFrameBuffer(GLuint frameBuffer);
		void setVAO(GLint id);
		void setProgram(GLint id);

		bool isProgramBound(render::bwo::Program const& p) const;
		bool isVAOBound(GLint id) const;

		void flushState();

		bool operator==(State const& other) const;

		State();
		~State() = default;

		NO_COPY_MOVE(State);
	};
}
