#include "oglt_device.h"

#include "std_util.h"

using namespace oglt;

static bool keyStates[256]{ false };
static device::Cursor cursor;

bool device::key(int ikey) {
	if(abs(ikey) < 256)
		return keyStates[ikey];
	return false;
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