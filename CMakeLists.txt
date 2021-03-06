# render - A C++ OpenGL library
# Copyright (C) 2021  Intrets
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
