#pragma once

#include "oglt_irenderable.h"
#include "oglt_material.h"
#include "oglt_mesh.h"
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

		void connectMtlToMesh(FbxMesh* fbxMesh, Mesh* ogltMesh);
		void loadMaterial(FbxMesh* mesh, Mesh* ogltMesh);
		void loadMaterialAttribute(FbxSurfaceMaterial* surfaceMaterial, Material* outMaterial);
		void loadMaterialTexture(FbxSurfaceMaterial* surfaceMaterial, Material* outMaterial);
		void loadTexture(FbxTexture* texture, MaterialParam param, Material* outMaterial);

		void finalizeVBO();

#define MAX_UV_CHANNEL 5

		bool loaded;
		vector<Mesh> meshs;
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