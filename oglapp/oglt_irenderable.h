#pragma once

#include "oglt_util.h"

namespace oglt {
	class IRenderable {
	public:
		virtual void Render(int renderType = OGLT_RENDER_SELF) {};
	protected:
		bool visiable = true;
	};
}