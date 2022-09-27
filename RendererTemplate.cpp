// render - A C++ OpenGL library
// Copyright (C) 2021 intrets

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
