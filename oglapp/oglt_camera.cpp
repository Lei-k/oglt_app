#include "std_util.h"
#include "oglt_device.h"
#include "oglt_camera.h"

#include <glm/gtx/rotate_vector.hpp>

using namespace oglt;
using namespace oglt::scene;

const float PI = float(atan(1.0)*4.0);

FlyingCamera::FlyingCamera()
{
	vEye = glm::vec3(0.0f, 0.0f, 0.0f);
	vView = glm::vec3(0.0f, 0.0, -1.0f);
	vUp = glm::vec3(0.0f, 1.0f, 0.0f);
	speed = 25.0f;
	sensitivity = 0.1f;
}

FlyingCamera::FlyingCamera(oglt::IApp* app, glm::vec3 a_vEye, glm::vec3 a_vView, glm::vec3 a_vUp, float a_fSpeed, float a_fSensitivity)
{
	this->app = app;
	vEye = a_vEye; vView = a_vView; vUp = a_vUp;
	speed = a_fSpeed;
	sensitivity = a_fSensitivity;
}

/*-----------------------------------------------

Name:	rotateWithMouse

Params:	none

Result:	Checks for moving of mouse and rotates
camera.

/*---------------------------------------------*/

void FlyingCamera::rotateWithMouse()
{
	int x, y;
	device::getCursor(x, y);
	uint w, h;
	app->getViewport(w, h);
	int iCentX = w / 2,
		iCentY = h / 2;

	float deltaX = (float)(iCentX - x)*sensitivity;
	float deltaY = (float)(iCentY - y)*sensitivity;

	if (abs(deltaX) > 0.01f)
	{
		vView -= vEye;
		vView = glm::rotate(vView, deltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		vView += vEye;
	}
	if (abs(deltaY) > 0.01f)
	{
		glm::vec3 vAxis = glm::cross(vView - vEye, vUp);
		vAxis = glm::normalize(vAxis);
		float fAngle = deltaY;
		float fNewAngle = fAngle + getAngleX();
		if (fNewAngle > -89.80f && fNewAngle < 89.80f)
		{
			vView -= vEye;
			vView = glm::rotate(vView, deltaY, vAxis);
			vView += vEye;
		}
	}
	device::setCursor(iCentX, iCentY);
}

/*-----------------------------------------------

Name:	GetAngleY

Params:	none

Result:	Gets Y angle of camera (head turning left
and right).

/*---------------------------------------------*/

float FlyingCamera::getAngleY()
{
	glm::vec3 vDir = vView - vEye; vDir.y = 0.0f;
	glm::normalize(vDir);
	float fAngle = acos(glm::dot(glm::vec3(0, 0, -1), vDir))*(180.0f / PI);
	if (vDir.x < 0)fAngle = 360.0f - fAngle;
	return fAngle;
}

/*-----------------------------------------------

Name:	GetAngleX

Params:	none

Result:	Gets X angle of camera (head turning up
and down).

/*---------------------------------------------*/

float FlyingCamera::getAngleX()
{
	glm::vec3 vDir = vView - vEye;
	vDir = glm::normalize(vDir);
	glm::vec3 vDir2 = vDir; vDir2.y = 0.0f;
	vDir2 = glm::normalize(vDir2);
	float fAngle = acos(glm::dot(vDir2, vDir))*(180.0f / PI);
	if (vDir.y < 0)fAngle *= -1.0f;
	return fAngle;
}

/*-----------------------------------------------

Name:	SetMovingKeys

Params:	a_iForw - move forward Key
a_iBack - move backward Key
a_iLeft - strafe left Key
a_iRight - strafe right Key

Result:	Sets Keys for moving camera.

/*---------------------------------------------*/

void FlyingCamera::setMovingKeys(int a_iForw, int a_iBack, int a_iLeft, int a_iRight)
{
	forwKey = a_iForw;
	backKey = a_iBack;
	leftKey = a_iLeft;
	rightKey = a_iRight;
}

/*-----------------------------------------------

Name:	Update

Params:	none

Result:	Performs updates of camera - moving and
rotating.

/*---------------------------------------------*/

void FlyingCamera::update()
{
	rotateWithMouse();

	// Get view direction
	glm::vec3 vMove = vView - vEye;
	vMove = glm::normalize(vMove);
	vMove *= speed;

	glm::vec3 vStrafe = glm::cross(vView - vEye, vUp);
	vStrafe = glm::normalize(vStrafe);
	vStrafe *= speed;

	int iMove = 0;
	glm::vec3 vMoveBy;
	// Get vector of move
	if (device::key(forwKey))vMoveBy += vMove * 1.0f * app->getDeltaTime();
	if (device::key(backKey))vMoveBy -= vMove * 1.0f * app->getDeltaTime();
	if (device::key(leftKey))vMoveBy -= vStrafe * 1.0f * app->getDeltaTime();
	if (device::key(rightKey))vMoveBy += vStrafe * 1.0f * app->getDeltaTime();
	vEye += vMoveBy; vView += vMoveBy;
}

/*-----------------------------------------------

Name:	ResetMouse

Params:	none

Result:	Sets mouse cursor back to the center of
window.

/*---------------------------------------------*/

void FlyingCamera::resetMouse()
{
	uint w, h;
	app->getViewport(w, h);
	device::setCursor((float)w / 2.0f, (float)h / 2.0f);
}

/*-----------------------------------------------

Name:	Look

Params:	none

Result:	Returns proper modelview matrix to make
camera look.

/*---------------------------------------------*/

glm::mat4 FlyingCamera::look()
{
	return glm::lookAt(vEye, vView, vUp);
}