#include "oglt_key.h"

using namespace oglt;

static bool keyStates[256];

bool keys::key(int ikey) {
	return keyStates[ikey];
}

void keys::setKey(int ikey, bool state) {
	keyStates[ikey] = state;
}