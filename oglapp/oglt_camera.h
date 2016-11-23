#pragma once

#include "oglt_scene_node.h"
#include "oglt_iapp.h"

namespace oglt {
	namespace scene {
		class FlyingCamera : SceneNode
		{
		public:
			float speed;
			float sensitivity; // How many degrees to rotate per pixel moved by mouse (nice value is 0.10)

								// Main functions
			void update();
			glm::mat4* look();

			void setMovingKeys(int a_iForw, int a_iBack, int a_iLeft, int a_iRight);
			void resetMouse();

			// Constructors
			FlyingCamera();
			FlyingCamera(oglt::IApp* app, glm::vec3 a_vEye, glm::vec3 a_vView, glm::vec3 a_vUp, float a_fSpeed, float a_fSensitivity);

		private:
			void rotateWithMouse();
			// Functions that get viewing angles
			float getAngleX(), getAngleY();

			int forwKey, backKey, leftKey, rightKey;

			glm::vec3 vView, vUp;
			glm::mat4 viewMatrix;

			oglt::IApp* app;
		};
	}
}