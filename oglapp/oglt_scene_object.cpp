#include "oglt_scene_object.h"

using namespace oglt;
using namespace oglt::scene;

SceneObject::SceneObject()
{
	
}

SceneObject::~SceneObject()
{
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

void SceneObject::render(int renderType)
{
	if(shaderProgram != NULL)
		shaderProgram->setUniform("matrices.modelMatrix", modelMatrix);

	FOR (i, renderObjs.size()) {
		renderObjs[i]->render();
	}

	if (renderType & OGLT_RENDER_CHILDREN) {
		if(brother != NULL)
			brother->render(renderType);

		if(child != NULL)
			child->render(renderType);
	}
}
