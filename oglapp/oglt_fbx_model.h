#pragma once

#include "oglt_irenderable.h"
#include "oglt_texture.h"
#include "oglt_vbo.h"

#include <fbxsdk.h>

namespace oglt {
	class FbxModel : public IRenderable {
	public:
		FbxModel();
		~FbxModel();

		static bool initialize();
		static void destroyManager();
		bool load(const string& fileName);

		virtual void render(int renderType = OGLT_RENDER_SELF);
	private:
		bool loadFromScene(FbxScene* scene);
		void traverseNode(FbxNode* parentNode, vector<glm::vec3>& vertices, vector<glm::vec2>& vus, vector<glm::vec3>& normals);

		struct MeshEntry {
			uint startIndex;
			uint size;
			uint materialIndex = OGLT_INVALID_MATERIAL_ID;
		};

		bool loaded;
		VertexBufferObject vboData;
		vector<MeshEntry> meshs;
		vector<Texture> textures;

		uint vao;

		static FbxManager* manager;
		FbxImporter* importer;
		FbxScene* scene;
	};
}