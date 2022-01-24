// render - A C++ OpenGL library
// Copyright (C) 2021  Intrets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "RendererTemplate.h"

namespace render
{
	render::ScopedVAO::ScopedVAO(GLuint id, bool resetOnDestruct_) : resetOnDestruct(resetOnDestruct_) {
#ifdef DEBUG_BUILD
		GLint p = 0;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &p);
		assert(static_cast<GLuint>(p) == current);
#endif
		if (current != id) {
			current = id;
			glBindVertexArray(id);
		}
	}

	ScopedVAO::~ScopedVAO() {
		if (this->resetOnDestruct) {
			current = 0;
			glBindVertexArray(0);
		}
	}
}
