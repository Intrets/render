// render - A C++ OpenGL library
// Copyright (C) 2021 intrets

#include "RendererTemplate.h"

#include <mem/Global.h>
#include "GLStateWrapper.h"

namespace render
{
	render::ScopedVAO::ScopedVAO(ogs::State& openglState_, GLuint id, bool resetOnDestruct_) : resetOnDestruct(resetOnDestruct_), openglState(openglState_) {
		this->openglState.setVAO(id);
	}

	ScopedVAO::~ScopedVAO() {
		if (this->resetOnDestruct) {
			this->openglState.setVAO(0);
		}
	}

	int UniformState::getTextureUnit() {
		return this->textureUnit++;
	}
}
