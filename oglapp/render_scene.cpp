#include "render_scene.h"

#include "oglt_util.h"
#include "std_util.h"

#include <GL/glew.h>

float timer = 0.0f;

void render::initScene(oglt::IApp* app) {
	glClearColor(0.1f, 0.3f, 0.7f, 1.0f);
}

void render::renderScene(oglt::IApp* app) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	timer += app->getDeltaTime();
	if (timer > 1.0f) {
		cout << "fps: " << app->getFps() << endl;
		cout << "delta time: " << app->getDeltaTime() << endl;
		timer -= 1.0f;
	}
	app->swapBuffers();
}

void render::releaseScene(oglt::IApp* app) {

}