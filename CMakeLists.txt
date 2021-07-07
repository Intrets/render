cmake_minimum_required(VERSION 3.20)

include("${CMAKE_SOURCE_DIR}/cmake/MakeModule.cmake")

set(MODULE_NAME render)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		Enums
		Colors
		BufferWrappers
		loaders/ShaderLoader
		loaders/ModelLoader
		loaders/TextureLoader
		loaders/vboindexer
		BlitRenderer
		UIBackgroundRenderer
		TextRenderer
		DebugRenderer
		BlitRendererArray
		infos/TextRenderInfo
		infos/UIRenderInfo
		infos/CameraInfo
		infos/TileRenderInfo
		infos/BlitArrayRenderInfo
		infos/BlitRenderInfo
		infos/DebugRenderInfo
		infos/HighlightRenderInfo
		infos/proxy/UIInfos
		Fonts
		textures/BlockIDTextures
		GLStateWrapper
		HighlightRenderer
	CXX_STANDARD 20
	REQUIRED_LIBS
		WGLM
		GLEW
		GLI
		GLFW3
		mem
		misc
)

set(SHADER_FILES
	Blit
	UIBackground
	BlitArray
	Debug
	Highlight
)

foreach(SHADER ${SHADER_FILES})
	set(FULL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER}")

	file(READ "${FULL_PATH}.frag" FRAGMENT_SHADER)
	file(WRITE "${FULL_PATH}.frag.inc" "static const char* ${SHADER}_frag = R\"(${FRAGMENT_SHADER})\";\n")

	file(READ "${FULL_PATH}.vert" VERTEX_SHADER)
	file(WRITE "${FULL_PATH}.vert.inc" "static const char* ${SHADER}_vert = R\"(${VERTEX_SHADER})\";\n")
endforeach()

target_include_directories(${MODULE_NAME} PRIVATE "./shaders")

