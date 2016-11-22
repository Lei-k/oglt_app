#include "oglt_app.h"

#include "glut_backend.h"
#include "oglt_cursor.h"

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
}

void OgltApp::keyboard(OGLT_KEY key, OGLT_KEY_STATE state){

}

void OgltApp::mouse(OGLT_BUTTON button, OGLT_BUTTON_STATE state, int x, int y) {
	
}

void OgltApp::mouseMotion(int x, int y) {
	cursor::setCursor(x, y);
}

void oglt::OgltApp::reshape(uint width, uint height)
{
	viewportWidth = width;
	viewportHeight = height;
	glViewport(0, 0, width, height);
	proj = glm::perspective(45.0f, (float)width / (float)height, 0.005f, 1000.0f);
	orth = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}

void oglt::OgltApp::swapBuffers()
{
	glutBackendSwapBuffers();
}
