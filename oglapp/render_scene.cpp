#include "render_scene.h"

#include "oglt_util.h"
#include "std_util.h"

#include "oglt_shader.h"
#include "oglt_freetypefont.h"
#include "oglt_skybox.h"
#include "oglt_camera.h"

using namespace oglt;
using namespace oglt::scene;

using namespace glm;

FreeTypeFont ftFont;
Skybox skybox;
FlyingCamera camera;

Shader ortho, font, vtMain, fgMain;
ShaderProgram spFont, spMain;

void scene::initScene(oglt::IApp* app) {
	glClearColor(0.1f, 0.3f, 0.7f, 1.0f);

	ortho.loadShader("data/shaders/ortho2D.vert", GL_VERTEX_SHADER);
	font.loadShader("data/shaders/font2D.frag", GL_FRAGMENT_SHADER);
	vtMain.loadShader("data/shaders/main_shader.vert", GL_VERTEX_SHADER);
	fgMain.loadShader("data/shaders/main_shader.frag", GL_FRAGMENT_SHADER);

	spFont.createProgram();
	spFont.addShaderToProgram(&ortho);
	spFont.addShaderToProgram(&font);
	spFont.linkProgram();

	spMain.createProgram();
	spMain.addShaderToProgram(&vtMain);
	spMain.addShaderToProgram(&fgMain);
	spMain.linkProgram();

	spMain.setUniform("sunLight.vColor", vec3(1.0f, 1.0f, 1.0f));
	spMain.setUniform("sunLight.vDirection", vec3(sqrt(2.0f) / 2, -sqrt(2.0f) / 2, 0));
	spMain.setUniform("sunLight.fAmbient", 0.5f);

	ftFont.loadFont("data/fonts/SugarpunchDEMO.otf", 32);
	ftFont.setShaderProgram(&spFont);
	
	skybox.load("data/skyboxes/elbrus/", "elbrus_front.jpg", "elbrus_back.jpg", "elbrus_left.jpb", "elbrus_right.jpg", "elbrus_top.jpg", "elbrus_bottom.jpg");
	skybox.setShaderProgram(&spMain);
	skybox.getLocalTransform()->scale = vec3(50, 50, 50);

	camera = FlyingCamera(app, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 30.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.01f);
	camera.setMovingKeys('w', 's', 'a', 'd');
}

void scene::renderScene(oglt::IApp* app) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera.update();

	spMain.useProgram();
	spMain.setUniform("matrices.viewMatrix", camera.look());
	spMain.setUniform("matrices.projMatrix", app->getProj());

	skybox.calcNodeHeirarchyTransform();
	skybox.render();

	spFont.useProgram();
	spFont.setUniform("matrices.projMatrix", app->getOrth());
	spFont.setUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	glDisable(GL_DEPTH_TEST);

	uint w, h;
	app->getViewport(w, h);
	ftFont.printFormatted(20, h - 35, 24, "FPS: %d", app->getFps());

	glEnable(GL_DEPTH_TEST);

	app->swapBuffers();
}

void scene::releaseScene(oglt::IApp* app) {
	spFont.deleteProgram();

	ortho.deleteShader();
	font.deleteShader();

	skybox.deleteSkybox();

	spMain.deleteProgram();
	vtMain.deleteShader();
	fgMain.deleteShader();
}