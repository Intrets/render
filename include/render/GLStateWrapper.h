// render - A C++ OpenGL library
// Copyright (C) 2021  Intrets
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

#include <cstdint>

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
	Configuration HighlightConfiguration();
	Configuration GeneralConfiguration();
	Configuration ShadowMapConfiguration();
	Configuration BlitConfiguration();
	Configuration NinesConfiguration();

	class State
	{
	public:
		Configuration configuration;

		int32_t MAX_COLOR_ATTACHMENTS;

		State();

		void setState(Configuration const& config);

		void setBlend(BLEND b);
		void setBlendFunc(BLEND_FUNC func);
		void setDepthTest(DEPTH_TEST test);
		void setDepthFunc(DEPTH_FUNC func);
		void setPolygonMode(POLYGON_MODE mode);

		void flushState();
	};
}
