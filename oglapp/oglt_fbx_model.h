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
		void updateAnimation(float deltaTime);

		virtual void render(int renderType = OGLT_RENDER_SELF);
	private:
		struct VertexBoneData {
			int boneIndices[4];
			float weights[4];

			VertexBoneData() {
				memset(boneIndices, 0, sizeof(boneIndices));
				memset(weights, 0, sizeof(weights));
			}

			void addBoneData(int boneIndex, float wieght);
		};

		struct BoneInfo {
			glm::mat4 boneOffset;
			glm::mat4 finalTransform;
		};

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

		void connectSkeletonToMesh(FbxMesh* fbxMesh, vector<VertexBoneData>& ctrlPointBones);
		void connectSkinToMesh(FbxSkin* skin, vector<VertexBoneData>& ctrlPointBones);
		void mapVertexBoneFromCtrlPoint(vector<VertexBoneData>& ctrlPointBones, vector<uint>& ctrlPointIndices);

		void readNodeCurve(FbxAnimLayer* animLayer, FbxAnimEvaluator* animEvaluator, FbxNode* node, FbxTime& time);

		void finalizeVBO();

		glm::mat4 toGlmMatrix(FbxAMatrix& matrix);
		glm::mat4 toGlmMatrix(FbxMatrix& matrix);

#define MAX_UV_CHANNEL 5

		bool loaded;
		vector<Mesh> meshs;
		vector<Texture> textures;

		uint vao;
		VertexBufferObject vertices;
		VertexBufferObject boneIndices;
		VertexBufferObject boneWeights;
		VertexBufferObject colors;
		VertexBufferObject normals;
		VertexBufferObject uvs[MAX_UV_CHANNEL];

		map<string, uint> boneMapping;
		vector<BoneInfo> boneInfos;

		float timer;

		static FbxManager* manager;
		FbxImporter* importer;
		FbxScene* scene;
	};
}