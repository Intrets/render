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

#include <mem/Global.h>
#include "GLStateWrapper.h"

namespace render
{
	render::ScopedVAO::ScopedVAO(ogs::State& openglState_, GLuint id, bool resetOnDestruct_) : openglState(openglState_), resetOnDestruct(resetOnDestruct_) {
		this->openglState.setVAO(id);
	}

	ScopedVAO::~ScopedVAO() {
		if (this->resetOnDestruct) {
			this->openglState.setVAO(0);
		}
	}
}
