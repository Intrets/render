#include "DebugRenderer.h"

#include "GLStateWrapper.h"
#include "infos/CameraInfo.h"
#include "infos/DebugRenderInfo.h"

#include <mem/Locator.h>

void render::DebugRenderer::render(
	DebugRenderInfo const& info,
	bwo::FrameBuffer& target,
	glm::ivec4 viewport,
	CameraInfo const& cameraInfo) {

	this->VAO.bind();
	this->program.use();

	// Draw debug points and lines in world space
	if (!info.world.lines.lines.empty()) {
		auto config = ogs::DebugLineConfiguration();
		Locator<ogs::State>::ref().setState(config);

		this->VP.set(cameraInfo.VP);

		this->points.set(info.world.lines.lines);

		target.draw(viewport, [&]() {
			glDrawArrays(
				GL_LINES,
				0,
				info.world.lines.lines.size()
			); }
		);
	}
	if (!info.world.points.points.empty()) {
		auto config = ogs::DebugPointConfiguration();

		Locator<ogs::State>::ref().setState(config);

		this->VP.set(cameraInfo.VP);

		this->points.set(info.world.points.points);

		target.draw(viewport, [&]() {
			glDrawArrays(
				GL_POINTS,
				0,
				info.world.points.points.size()
			); }
		);
	}
}