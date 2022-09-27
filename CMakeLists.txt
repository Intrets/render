# render - A C++ OpenGL library
# Copyright (C) 2021 intrets

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
		loaders/ModelLoader
		Colors
		SRGBConversion
	CXX_STANDARD 20
	REQUIRED_LIBS
		mem
		WGLM
		misc
		gli
		wrangled_gl
		resource_data
)
