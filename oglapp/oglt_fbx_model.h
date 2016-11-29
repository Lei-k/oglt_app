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
		void processNode(FbxNode* node);
		void processMesh(FbxNode* node);
		void processSkeleton(FbxNode* node);
		void processLight(FbxNode* node);
		void processCamera(FbxNode* node);

		void readVertex(FbxMesh* mesh, int ctrlPointIndex, glm::vec3* outVertex);
		void readColor(FbxMesh* mesh, int ctrlPointIndex, int vertexCounter, glm::vec4* outColor);
		void readUV(FbxMesh* mesh, int ctrlPointIndex, int textureUVIndex, int uvLayer, glm::vec2* outUV);
		void readNormal(FbxMesh* mesh, int ctrlPointIndex, int vertexCounter, glm::vec3* outNormal);
		void readTangent(FbxMesh* mesh, int ctrlPointIndex, int vertexCounter, glm::vec3* outTangent);

		void finalizeVBO();

		struct MeshEntry {
			int startIndex;
			int size;
			int materialIndex = OGLT_INVALID_MATERIAL_ID;
		};

#define MAX_UV_CHANNEL 5

		bool loaded;
		vector<MeshEntry> meshs;
		vector<Texture> textures;

		uint vao;
		VertexBufferObject vertices;
		VertexBufferObject colors;
		VertexBufferObject normals;
		VertexBufferObject uvs[MAX_UV_CHANNEL];

		static FbxManager* manager;
		FbxImporter* importer;
		FbxScene* scene;
	};
}