#pragma once

#include <wglm/glm.hpp>
#include <vector>

namespace render
{
	struct SingleBlitRenderInfo
	{
		glm::vec4 quad;
		glm::vec4 world;
		int32_t rotation;

		static SingleBlitRenderInfo full() {
			return SingleBlitRenderInfo{
				.quad = {0.0f, 0.0f, 1.0f, 1.0f},
				.world = {-1.0f, -1.0f, 2.0f, 2.0f},
				.rotation = 0
			};
		}
	};

	struct BlitRenderInfo
	{
	private:
		std::vector<SingleBlitRenderInfo> data;

	public:
		std::vector<SingleBlitRenderInfo> const& getData() const;

		std::vector<SingleBlitRenderInfo>& getDataMut();

		void addBlitInfo(glm::vec4 q, glm::vec4 w, int32_t r);
		void addBlitInfo(SingleBlitRenderInfo const& info);
	};
}