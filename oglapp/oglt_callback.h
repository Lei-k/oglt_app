#pragma once

namespace oglt {
	class ICallback {
	public:
		virtual void renderScene() {}
		virtual void keyboard(OGLT_KEY key, OGLT_KEY_STATE state) {}
		virtual void mouse(OGLT_BUTTON button, OGLT_BUTTON_STATE state, int x, int y) {}
		virtual void reshape(uint width, uint height){}
	};
}