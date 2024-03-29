// render - A C++ OpenGL library
// Copyright (C) 2022 intrets

#pragma once

#include <wrangled_gl/wrangled_gl.h>

#include <wglm/vec4.hpp>

#include <tepp/optional_ref.h>

#include <misc/Misc.h>

#include <cstdint>
#include <optional>
#include <unordered_map>

enum class DEPTH_MASK
{
	UNSET,
	FALSE,
	TRUE,
	MAX,
};

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
	SEPERATE____GL_SRC_ALPHA__GL_ONE_MINUS_SRC_ALPHA___GL_ONE_MINUS_DST_ALPHA__GL_ONE,
	ONE__ZERO,
	ZERO__ONE,
	ONE__ONE,
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
	ALWAYS,
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
	class FrameBuffer;
}

namespace ogs
{
	class State;

	struct Configuration
	{
		BLEND blend = BLEND::UNSET;
		BLEND_FUNC blendFunc = BLEND_FUNC::UNSET;
		DEPTH_TEST depthTest = DEPTH_TEST::UNSET;
		DEPTH_FUNC depthFunc = DEPTH_FUNC::UNSET;
		POLYGON_MODE polygonMode = POLYGON_MODE::UNSET;
		float pointSize = 0.0f;
		DEPTH_MASK depthMask = DEPTH_MASK::UNSET;
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
	Configuration ParticleConfiguration();
	Configuration BadBloomConfiguration();

	class ProgramRegistry
	{
	public:
		std::unordered_map<GLint, render::bwo::Program*> programs;

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
		void setDepthMask(DEPTH_MASK b);

		void setViewport(glm::ivec4 p);
		void setFrameBuffer(GLuint frameBuffer);
		void setFrameBuffer(render::bwo::FrameBuffer const& frameBuffer);
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
