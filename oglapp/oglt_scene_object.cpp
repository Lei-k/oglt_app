#include "oglt_scene_object.h"

using namespace oglt;
using namespace oglt::scene;

SceneObject::SceneObject()
{
	shaderProgram = NULL;
}

SceneObject::~SceneObject()
{
}

void SceneObject::setShaderProgram(ShaderProgram * shaderProgram)
{
	this->shaderProgram = shaderProgram;
}

void SceneObject::addRenderObj(oglt::IRenderable * renderObj)
{
	renderObjs.push_back(renderObj);
}

void SceneObject::removeRenderObj(oglt::IRenderable * renderObj)
{
	vector<oglt::IRenderable*>::iterator find_t = find(renderObjs.begin(), renderObjs.end(), renderObj);
	if (find_t != renderObjs.end()) {
		renderObjs.erase(find_t);
	}
}

void SceneObject::Render(int renderType)
{
	FOR (i, renderObjs.size()) {
		renderObjs[i]->Render();
	}

	renderType |= OGLT_RENDER_SELF;

	if (renderType & OGLT_RENDER_CHILDREN) {
		child->Render(renderType);
	}
}
