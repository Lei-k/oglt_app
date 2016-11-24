#include "oglt_app.h"

#include "glut_backend.h"
#include "oglt_device.h"

#include "render_scene.h"

#include <glut.h>
#include <glm\gtc\matrix_transform.hpp>

using namespace oglt;

OgltApp::OgltApp() {
	
}

OgltApp::~OgltApp() {
	scene::releaseScene(this);
}

void OgltApp::init(int argc, char** argv, int ogltOptions) {
	IApp::init();
	glutBackendInit(argc, argv, ogltOptions);
}

bool OgltApp::createWindow(uint width, uint height, const char* title, int ogltOptions) {
	return glutBackendCreateWindow(width, height, title, ogltOptions);
}

void OgltApp::run() {
	scene::initScene(this);
	glutBackendRun(this);
}

void OgltApp::renderScene() {
	updateTimer();
	scene::renderScene(this);
	int x, y;
	device::getCursor(x, y);
	glutBackendSetCursor(x, y);
}

void OgltApp::keyboard(OGLT_KEY key, OGLT_KEY_STATE state){
	switch (state) {
	case OGLT_KEY_PRESS:
		device::setKey(key, true);
		if (key == 'q') {
			exit(0);
		}
		break;
	case OGLT_KEY_RELEASE:
		device::setKey(key, false);
		break;
	}
}

void OgltApp::mouse(OGLT_BUTTON button, OGLT_BUTTON_STATE state, int x, int y) {
	
}

void OgltApp::mouseMotion(int x, int y) {
	device::setCursor(x, y);
}

void oglt::OgltApp::reshape(uint width, uint height)
{
	viewportWidth = width;
	viewportHeight = height;
	glViewport(0, 0, width, height);
	proj = glm::perspective(45.0f, (float)width / (float)height, 0.005f, 10000.0f);
	orth = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}

void oglt::OgltApp::swapBuffers()
{
	glutBackendSwapBuffers();
}
