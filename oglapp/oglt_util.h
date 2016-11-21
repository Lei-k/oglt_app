#pragma once

namespace oglt {

// define oglt options
#define OGLT_NONE 0x0000
#define OGLT_DEPTH 0x0001
#define OGLT_STENCIL  0x0002
#define OGLT_FULLSCREEN 0x0004

	typedef unsigned int uint;

	enum OGLT_BUTTON {
		OGLT_BUTTON_RIGHT,
		OGLT_BUTTON_MIDDLE,
		OGLT_BUTTON_LEFT,
		OGLT_BUTTON_UNDIFINED = 999
	};

	enum OGLT_BUTTON_STATE {
		OGLT_BUTTON_DOWN,
		OGLT_BUTTON_UP,
		OGLT_BUTTON_STATE_UNDEFINED = 999
	};

	enum OGLT_KEY {

	};

	enum OGLT_KEY_STATE {

	};
}