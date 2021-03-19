#pragma once

#include <wglm/glm.hpp>
#include <optional>

#include "BufferWrappers.h"
#include "infos/BlitRenderInfo.h"
#include "GLStateWrapper.h"

#include "../shaders/Blit.vert.inc"
#include "../shaders/Blit.frag.inc"

namespace render
{
	class BlitRenderer
	{
	private:
		bwo::Program program{ Blit_vert, Blit_frag, "BlitRenderer" };

		bwo::ArrayBuffer<glm::vec2> quad{ bwo::BufferHint::STATIC_DRAW };
		bwo::ArrayBuffer<SingleBlitRenderInfo> infos{ bwo::BufferHint::STREAM_DRAW };

		bwo::VertexArrayObject<
			bwo::Group<glm::vec2, 0,
			bwo::VInfo<float, 2>>,

			bwo::Group<SingleBlitRenderInfo, 1,
			bwo::VInfo<float, 4>,
			bwo::VInfo<float, 4>,
			bwo::VInfo<int32_t, 1>>
			> VAO{ quad, infos };

		bwo::UniformTexture2D texture_t{ "texture_t", program,0 };
		bwo::Uniform2fv UVflip{ "UVflip", program };
		bwo::Uniform1f depth{ "depth", program };
		bwo::Uniform2fv offset{ "offset", program };
		bwo::Uniform4fv color{ "c", program };

	public:
		int32_t const MAX_BATCH_SIZE = 5000;

		void render(ogs::Configuration const& config,
					BlitRenderInfo const& blitInfos,
					bwo::FrameBuffer& target,
					glm::ivec4 viewport,
					bwo::Texture2D const& texture,
					std::optional<float> depth_,
					bool flipUVvertical,
					glm::vec2 offset,
					std::optional<glm::vec4> color = std::nullopt);

		void render(ogs::Configuration const&,
					SingleBlitRenderInfo const& blitInfo,
					bwo::FrameBuffer& target,
					glm::ivec4 viewport,
					bwo::Texture2D const& texture,
					std::optional<float> depth_,
					bool fromTexture,
					std::optional<glm::vec4> color = std::nullopt);

		BlitRenderer();
		~BlitRenderer();
	};
}
