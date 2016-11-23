#pragma once

namespace oglt {
	namespace device {
		struct Cursor {
			int x, y;
		};

		void setCursor(int x, int y);
		void getCursor(int& x, int& y);

		bool key(int ikey);
		void setKey(int ikey, bool state);
	}
}