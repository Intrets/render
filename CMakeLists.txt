cmake_minimum_required(VERSION 3.20)

include("${CMAKE_SOURCE_DIR}/cmake/MakeModule.cmake")

set(MODULE_NAME render)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		BufferWrappers
		GLStateWrapper
		RendererTemplate
		loaders/ShaderLoader
		loaders/TextureLoader
		Colors
		SRGBConversion
	CXX_STANDARD 20
	REQUIRED_LIBS
		mem
		WGLM
		misc
		gli
		GLEW
		resource_data
)

add_executable(${MODULE_NAME}_test_exe Main.cpp)

target_link_libraries(${MODULE_NAME}_test_exe
	PUBLIC
		${MODULE_NAME}
		tepp
		opengl32
		shaders
)
