#pragma once

#include "oglt_util.h"
#include "oglt_shader.h"

namespace oglt {
	class IRenderable {
	public:
		virtual void render(int renderType = OGLT_RENDER_SELF) {}
		virtual void setShaderProgram(ShaderProgram* shaderProgram){
			this->shaderProgram = shaderProgram;
		}

	protected:
		bool visiable = true;
		ShaderProgram* shaderProgram;
	};
}