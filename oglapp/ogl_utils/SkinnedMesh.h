#pragma once

#include "texture.h"
#include "shaders.h"
#include "vertexBufferObject.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

class SkinnedMesh {
public:
	SkinnedMesh();
	~SkinnedMesh();

	bool LoadMesh(const string& fileName);

	void Render();

	uint NumBones() const {
		return numBones;
	}

	void BoneTransform(float deltaTime, vector<glm::mat4>& transforms);

private:
#define NUM_BONES_PER_VEREX 4
#define INVALID_MATERIAL 0xFFFFFFFF
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
	
	struct BoneInfo {
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransform;

		BoneInfo() {
			BoneOffset = glm::mat4(0);
			FinalTransform = glm::mat4(0);
		}
	};

	struct VertexBoneData {
		uint IDs[NUM_BONES_PER_VEREX];
		float Weights[NUM_BONES_PER_VEREX];

		VertexBoneData() {
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);
		}

		void AddBoneData(UINT boneId, float weight);
	};

	struct MeshEntry {
		uint StartIndex;
		uint Size;
		uint MaterialIndex;

		MeshEntry() {
			StartIndex = 0;
			Size = 0;
			MaterialIndex = INVALID_MATERIAL;
		}
	};

	string GetDirectoryPath(string sFilePath);
	void CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
	uint FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
	uint FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
	uint FindPosition(float animationTime, const aiNodeAnim* nodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const string nodeName);
	void ReadNodeHeirarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform);
	bool InitFromScene(const aiScene* scene, const string& fileName);
	glm::mat4 ConverAssimpMatrix(const aiMatrix4x4& aiMatrix);
	glm::mat4 ConverAssimpMatrix(const aiMatrix3x3& aiMatrix);
	void FinalizeVBO();
	void Clear();

	float timer;

	bool loaded;
	uint vao;
	uint numBones;
	glm::mat4 globalInverseTransform;

	CVertexBufferObject modelData;

	vector<MeshEntry> meshs;
	vector<CTexture> textures;
	vector<BoneInfo> boneInfos;
	map<string, uint> boneMapping;

	const aiScene* scene;
	Assimp::Importer importer;
};