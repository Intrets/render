#pragma once

#include <wglm/glm.hpp>
#include <wglm/gtx/transform.hpp>

namespace render
{
	struct CameraInfo
	{
		int32_t x;
		int32_t y;
		glm::vec3 camPos;
		glm::mat4 rotation;
		glm::mat4 P;

		glm::vec3 viewPort;

		glm::mat4 VP() const {
			return P * glm::transpose(rotation) * glm::translate(-this->camPos);
		}
	};
}
