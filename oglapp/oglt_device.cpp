#include "oglt_device.h"

using namespace oglt;

static bool keyStates[256];
static device::Cursor cursor;

bool device::key(int ikey) {
	return keyStates[ikey];
}

void device::setKey(int ikey, bool state) {
	keyStates[ikey] = state;
}

void device::setCursor(int x, int y) {
	cursor.x = x;
    cursor.y = y;
}

void device::getCursor(int& x, int& y) {
	x = cursor.x;
	y = cursor.y;
}