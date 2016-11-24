#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>

#include "oglt_irenderable.h"

namespace oglt {
	namespace scene {
		struct Transform {
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
		};

		class SceneNode : IRenderable {
		public:
			SceneNode();
			~SceneNode();
			Transform* getLocalTransform();
			Transform* getWorldTransform();
			glm::mat4* getModelMatrix();

			void calcNodeHeirarchyTransform();

			void addChild(SceneNode* child);
			void removeChild(SceneNode* child);
			bool isRootNode();

			virtual void render(int renderType = OGLT_RENDER_SELF){}
		protected:
			Transform localTransform;
			Transform worldTransform;

			glm::mat4 modelMatrix;

			SceneNode* parent;
			SceneNode* child;
			SceneNode* brother;
		};
	}
}