#include "../std_util.h"

#include "flyingCamera.h"

#include "../oglt_device.h"

#include <glm/gtx/rotate_vector.hpp>

using namespace oglt;

const float PI = float(atan(1.0)*4.0);

CFlyingCamera::CFlyingCamera()
{
	vEye = glm::vec3(0.0f, 0.0f, 0.0f);
	vView = glm::vec3(0.0f, 0.0, -1.0f);
	vUp = glm::vec3(0.0f, 1.0f, 0.0f);
	fSpeed = 25.0f;
	fSensitivity = 0.1f;
}

CFlyingCamera::CFlyingCamera(oglt::IApp* pApp, glm::vec3 a_vEye, glm::vec3 a_vView, glm::vec3 a_vUp, float a_fSpeed, float a_fSensitivity)
{
	this->pApp = pApp;
	vEye = a_vEye; vView = a_vView; vUp = a_vUp;
	fSpeed = a_fSpeed;
	fSensitivity = a_fSensitivity;
}

/*-----------------------------------------------

Name:	rotateWithMouse

Params:	none

Result:	Checks for moving of mouse and rotates
		camera.

/*---------------------------------------------*/

void CFlyingCamera::RotateWithMouse()
{
	int x, y;
	device::getCursor(x, y);
	uint w, h;
	pApp->getViewport(w, h);
	int iCentX = w / 2,
	    iCentY = h / 2;

	float deltaX = (float)(iCentX- x)*fSensitivity;
	float deltaY = (float)(iCentY- y)*fSensitivity;

	if(abs(deltaX) > 0.01f)
	{
		vView -= vEye;
		vView = glm::rotate(vView, deltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		vView += vEye;
	}
	if(abs(deltaY) > 0.01f)
	{
		glm::vec3 vAxis = glm::cross(vView-vEye, vUp);
		vAxis = glm::normalize(vAxis);
		float fAngle = deltaY;
		float fNewAngle = fAngle+GetAngleX();
		if(fNewAngle > -89.80f && fNewAngle < 89.80f)
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

float CFlyingCamera::GetAngleY()
{
	glm::vec3 vDir = vView-vEye; vDir.y = 0.0f;
	glm::normalize(vDir);
	float fAngle = acos(glm::dot(glm::vec3(0, 0, -1), vDir))*(180.0f/PI);
	if(vDir.x < 0)fAngle = 360.0f-fAngle;
	return fAngle;
}

/*-----------------------------------------------

Name:	GetAngleX

Params:	none

Result:	Gets X angle of camera (head turning up
		and down).

/*---------------------------------------------*/

float CFlyingCamera::GetAngleX()
{
	glm::vec3 vDir = vView-vEye;
	vDir = glm::normalize(vDir);
	glm::vec3 vDir2 = vDir; vDir2.y = 0.0f;
	vDir2 = glm::normalize(vDir2);
	float fAngle =  acos(glm::dot(vDir2, vDir))*(180.0f/PI);
	if(vDir.y < 0)fAngle *= -1.0f;
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

void CFlyingCamera::SetMovingKeys(int a_iForw, int a_iBack, int a_iLeft, int a_iRight)
{
	iForw = a_iForw;
	iBack = a_iBack;
	iLeft = a_iLeft;
	iRight = a_iRight;
}

/*-----------------------------------------------

Name:	Update

Params:	none

Result:	Performs updates of camera - moving and
		rotating.

/*---------------------------------------------*/

void CFlyingCamera::Update()
{
	RotateWithMouse();

	// Get view direction
	glm::vec3 vMove = vView-vEye;
	vMove = glm::normalize(vMove);
	vMove *= fSpeed;

	glm::vec3 vStrafe = glm::cross(vView-vEye, vUp);
	vStrafe = glm::normalize(vStrafe);
	vStrafe *= fSpeed;

	int iMove = 0;
	glm::vec3 vMoveBy;
	// Get vector of move
	if(device::key(iForw))vMoveBy += vMove * 1.0f * pApp->getDeltaTime();
	if(device::key(iBack))vMoveBy -= vMove * 1.0f * pApp->getDeltaTime();
	if(device::key(iLeft))vMoveBy -= vStrafe * 1.0f * pApp->getDeltaTime();
	if(device::key(iRight))vMoveBy += vStrafe * 1.0f * pApp->getDeltaTime();
	vEye += vMoveBy; vView += vMoveBy;
}

/*-----------------------------------------------

Name:	ResetMouse

Params:	none

Result:	Sets mouse cursor back to the center of
		window.

/*---------------------------------------------*/

void CFlyingCamera::ResetMouse()
{
	
}

/*-----------------------------------------------

Name:	Look

Params:	none

Result:	Returns proper modelview matrix to make
		camera look.

/*---------------------------------------------*/

glm::mat4 CFlyingCamera::Look()
{
	return glm::lookAt(vEye, vView, vUp);
}