#include "render_scene.h"

#include "oglt_util.h"
#include "std_util.h"

#include "ogl_utils\shaders.h"
#include "ogl_utils\freeTypeFont.h"
#include "ogl_utils\flyingCamera.h"

#include <GL/glew.h>

CShader shOrtho2D, shFont2D;
CShaderProgram spFont2D;

CFreeTypeFont ftFont;

CFlyingCamera camera;

float timer = 0.0f;

float fTriangle[9]; // Data to render triangle (3 vertices, each has 3 floats)
float fQuad[12]; // Data to render quad using triangle strips (4 vertices, each has 3 floats)
float fTriangleColor[9];
float fQuadColor[12];

UINT uiVBO[4];
UINT uiVAO[2];

CShader shVertex, shFragment;
CShaderProgram spMain;

using namespace oglt;

void scene::initScene(oglt::IApp* app) {
	glClearColor(0.1f, 0.3f, 0.7f, 1.0f);

	fTriangle[0] = -0.4f; fTriangle[1] = 0.1f; fTriangle[2] = 0.0f;
	fTriangle[3] = 0.4f; fTriangle[4] = 0.1f; fTriangle[5] = 0.0f;
	fTriangle[6] = 0.0f; fTriangle[7] = 0.7f; fTriangle[8] = 0.0f;

	// Setup triangle color

	fTriangleColor[0] = 1.0f; fTriangleColor[1] = 0.0f; fTriangleColor[2] = 0.0f;
	fTriangleColor[3] = 0.0f; fTriangleColor[4] = 1.0f; fTriangleColor[5] = 0.0f;
	fTriangleColor[6] = 0.0f; fTriangleColor[7] = 0.0f; fTriangleColor[8] = 1.0f;

	// Setup quad vertices

	fQuad[0] = -0.2f; fQuad[1] = -0.1f; fQuad[2] = 0.0f;
	fQuad[3] = -0.2f; fQuad[4] = -0.6f; fQuad[5] = 0.0f;
	fQuad[6] = 0.2f; fQuad[7] = -0.1f; fQuad[8] = 0.0f;
	fQuad[9] = 0.2f; fQuad[10] = -0.6f; fQuad[11] = 0.0f;

	// Setup quad color

	fQuadColor[0] = 1.0f; fQuadColor[1] = 0.0f; fQuadColor[2] = 0.0f;
	fQuadColor[3] = 0.0f; fQuadColor[4] = 1.0f; fQuadColor[8] = 0.0f;
	fQuadColor[6] = 0.0f; fQuadColor[7] = 0.0f; fQuadColor[5] = 1.0f;
	fQuadColor[9] = 1.0f; fQuadColor[10] = 1.0f; fQuadColor[11] = 0.0f;

	glGenVertexArrays(2, uiVAO); // Generate two VAOs, one for triangle and one for quad
	glGenBuffers(4, uiVBO); // And four VBOs

							// Setup whole triangle
	glBindVertexArray(uiVAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), fTriangle, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), fTriangleColor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Setup whole quad
	glBindVertexArray(uiVAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[2]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), fQuad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[3]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), fQuadColor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Load shaders and create shader program

	shVertex.LoadShader("data\\shaders\\shader.vert", GL_VERTEX_SHADER);
	shFragment.LoadShader("data\\shaders\\shader.frag", GL_FRAGMENT_SHADER);

	spMain.CreateProgram();
	spMain.AddShaderToProgram(&shVertex);
	spMain.AddShaderToProgram(&shFragment);

	spMain.LinkProgram();

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	shOrtho2D.LoadShader("data\\shaders\\ortho2D.vert", GL_VERTEX_SHADER);
	shFont2D.LoadShader("data\\shaders\\font2D.frag", GL_FRAGMENT_SHADER);

	spFont2D.CreateProgram();
	spFont2D.AddShaderToProgram(&shOrtho2D);
	spFont2D.AddShaderToProgram(&shFont2D);
	spFont2D.LinkProgram();

	// Here we load font with pixel size 32 - this means that if we print with size above 32, the quality will be low
	if (!ftFont.LoadSystemFont("arial.ttf", 32)) {
		cout << "can not load font" << endl;
	}
	ftFont.SetShaderProgram(&spFont2D);

    camera = CFlyingCamera(app, glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 10.0f, 19.0f), glm::vec3(0.0f, 1.0f, 0.0f), 25.0f, 0.01f);
	camera.SetMovingKeys('W', 'S', 'A', 'D');
}

void scene::renderScene(oglt::IApp* app) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	spMain.UseProgram();
	glBindVertexArray(uiVAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(uiVAO[1]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	spFont2D.UseProgram();
	glDisable(GL_DEPTH_TEST);
	spFont2D.SetUniform("matrices.projMatrix", app->getOrth());
	spFont2D.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	uint w, h;
	app->getViewport(w, h);
	ftFont.PrintFormatted(20, h - 30, 20, "FPS: %d", app->getFps());
	ftFont.Print("lei-k oglt app", 20, 20, 24);
	glEnable(GL_DEPTH_TEST);

	app->swapBuffers();
}

void scene::releaseScene(oglt::IApp* app) {
	ftFont.DeleteFont();
	spFont2D.DeleteProgram();
	spMain.DeleteProgram();

	shVertex.DeleteShader();
	shFragment.DeleteShader();
	shFont2D.DeleteShader();
	shOrtho2D.DeleteShader();
}