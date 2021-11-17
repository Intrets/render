cmake_minimum_required(VERSION 3.20)

include("${CMAKE_SOURCE_DIR}/cmake/MakeModule.cmake")

set(MODULE_NAME render)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		BufferWrappers
		GLStateWrapper
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
)

set(SHADER_FILES
)

foreach(SHADER ${SHADER_FILES})
	set(FULL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER}")

	file(READ "${FULL_PATH}.frag" FRAGMENT_SHADER)
	file(WRITE "${FULL_PATH}.frag.inc" "static const char* ${SHADER}_frag = R\"(${FRAGMENT_SHADER})\";\n")

	file(READ "${FULL_PATH}.vert" VERTEX_SHADER)
	file(WRITE "${FULL_PATH}.vert.inc" "static const char* ${SHADER}_vert = R\"(${VERTEX_SHADER})\";\n")
endforeach()

target_compile_definitions(${MODULE_NAME}
    PUBLIC
        JUCE_OPENGL3=1
)

target_include_directories(${MODULE_NAME} PRIVATE "./shaders")

