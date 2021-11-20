#include <iostream>

#include <concepts>

#include "render/RendererTemplate.h"

#include <shaders.h>

int main() {
	using namespace render;

	Uniform1f test{ "uniform name test" };
	UniformTexture2D tex{ "texture" };

	//tex.

	//TEMPLATES::RenderInfo<Circle> ok;

	//TEMPLATES::Renderer<Circle, Uniforms> renderer(
	//	getBufferGenerator(shaders_enum::Circle_vert),
	//	getBufferGenerator(shaders_enum::Circle_frag),
	//	"CircleRenderer"
	//);

	//renderer.render(
	//	{},
	//	{
	//		.depth = 1.0f,
	//		.borderSize = 1.0f,
	//		.texture_t = tex,
	//		.texturePixelSize = 0.1f,
	//		.worldPixelSize = 1.0f,
	//		.VP = glm::mat4()
	//	}
	//	);

	render::ArrayBuffer<Circle, 1> buffer;
	render::VAO vao(buffer, buffer, buffer);

	rand();

	//renderer.uniforms.

	//renderer.render()

	//test.add()



	rand();

	return 0;
}