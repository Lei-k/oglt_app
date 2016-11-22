#include "common_header.h"

#include "SkinnedMesh.h"

#include <glm\gtc\matrix_transform.hpp>

void SkinnedMesh::VertexBoneData::AddBoneData(uint boneId, float weight)
{
	for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = boneId;
			Weights[i] = weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	assert(0);
}

SkinnedMesh::SkinnedMesh()
{
	vao = 0;
	numBones = 0;
	timer = 0.0f;
	scene = NULL;
}


SkinnedMesh::~SkinnedMesh()
{
	Clear();
}

void SkinnedMesh::Clear()
{
	if (vao != 0) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	meshs.clear();
	boneInfos.clear();
	textures.clear();
	boneMapping.clear();
}

string SkinnedMesh::GetDirectoryPath(string sFilePath)
{
	// Get directory path
	string sDirectory = "";
	RFOR(i, ESZ(sFilePath) - 1)if (sFilePath[i] == '\\' || sFilePath[i] == '/')
	{
		sDirectory = sFilePath.substr(0, i + 1);
		break;
	}
	return sDirectory;
}

bool SkinnedMesh::LoadMesh(const string& fileName) {
	Clear();

	modelData.CreateVBO();

	scene = importer.ReadFile(fileName.c_str(), ASSIMP_LOAD_FLAGS);

	bool check = false;

	if (!scene)
	{
		check = false;
	}
	else {
		globalInverseTransform = glm::inverse(ConverAssimpMatrix(scene->mRootNode->mTransformation));
		check = InitFromScene(scene, fileName);
	}

	return check;
}

bool SkinnedMesh::InitFromScene(const aiScene* scene, const std::string& fileName) {
	vector<aiVector3D> positions;
	vector<aiVector2D> uvs;
	vector<aiVector3D> normals;
	vector<VertexBoneData> vertexBones;

	uint numVertices = 0;

	for (uint i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		for (uint j = 0; j < mesh->mNumFaces; j++) {
			const aiFace& face = scene->mMeshes[i]->mFaces[j];
			for (uint k = 0; k < 3; k++) {
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
				positions.push_back(pos);
				uvs.push_back(aiVector2D(uv.x, uv.y));
				normals.push_back(normal);
			}
		}
		numVertices += mesh->mNumVertices;
	}

	vertexBones.resize(numVertices);
	numVertices = 0;
	for (uint i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		for (uint j = 0; j < mesh->mNumBones; j++) {
			uint boneIndex = 0;
			string boneName(mesh->mBones[j]->mName.data);
			if (boneMapping.find(boneName) == boneMapping.end()) {
				boneIndex = numBones;
				numBones++;
				BoneInfo bi;
				bi.BoneOffset = ConverAssimpMatrix(mesh->mBones[j]->mOffsetMatrix);
				boneInfos.push_back(bi);
				boneMapping[boneName] = boneIndex;
			}
			else {
				boneIndex = boneMapping[boneName];
			}

			for (uint k = 0; k < mesh->mBones[j]->mNumWeights; k++) {
				uint vertexId = numVertices + mesh->mBones[j]->mWeights[k].mVertexId;
				float weight = mesh->mBones[j]->mWeights[k].mWeight;
				vertexBones[vertexId].AddBoneData(boneIndex, weight);
			}
		}
		numVertices += mesh->mNumVertices;
	}

	meshs.resize(scene->mNumMeshes);

	const uint vertexTotalSize = sizeof(aiVector3D) * 2 + sizeof(aiVector2D) + sizeof(VertexBoneData);
	numVertices = 0;
	for (uint i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		meshs[i].MaterialIndex = mesh->mMaterialIndex;
		uint beforeSize = modelData.GetCurrentSize();
		meshs[i].StartIndex = beforeSize / vertexTotalSize;
		while (numVertices < mesh->mNumVertices) {
			modelData.AddData(&positions[numVertices], sizeof(aiVector3D));
			modelData.AddData(&uvs[numVertices], sizeof(aiVector2D));
			modelData.AddData(&normals[numVertices], sizeof(aiVector3D));
			modelData.AddData(&vertexBones[numVertices], sizeof(VertexBoneData));
			numVertices++;
		}
		meshs[i].Size = (modelData.GetCurrentSize() - beforeSize) / vertexTotalSize;
	}

	vector<uint> materialRemap(scene->mNumMaterials);
	string sDir = GetDirectoryPath(fileName);
	for(uint i = 0 ; i < scene->mNumMaterials ; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int texIndex = 0;
		aiString path;  // filename
		
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
			{
				string sTextureName(path.data);
				string sFullPath = sDir + sTextureName;
				int iTexFound = -1;
				for (int j = 0; j < textures.size(); j++) {
					if (sFullPath == tTextures[j].GetPath()) {
						iTexFound = j;
						break;
					}
				}
				if (iTexFound != -1) materialRemap[i] = iTexFound;
				else
				{
					CTexture tNew;
					tNew.LoadTexture2D(sFullPath, true);
					materialRemap[i] = textures.size();
					textures.push_back(tNew);
				}
			}
		}
	}

	for(uint i = 0 ; i < meshs.size() ; i++)
	{
		uint oldIndex = meshs[i].MaterialIndex;
		meshs[i].MaterialIndex = materialRemap[oldIndex];
	}

	FinalizeVBO();

	return true;
}

void SkinnedMesh::FinalizeVBO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	modelData.BindVBO();
	modelData.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D) + sizeof(VertexBoneData), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D) + sizeof(VertexBoneData), (void*)sizeof(aiVector3D));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D) + sizeof(VertexBoneData), (void*)(sizeof(aiVector3D) + sizeof(aiVector2D)));
	// Vertex Bone Data
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_INT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D) + sizeof(VertexBoneData), (void*)(2 * sizeof(aiVector3D) + sizeof(aiVector2D)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D) + sizeof(VertexBoneData), (void*)(2 * sizeof(aiVector3D) + sizeof(aiVector2D) + 4 * sizeof(uint)));
}

void SkinnedMesh::Render() {
	glBindVertexArray(vao);
	for (uint i = 0; i < meshs.size(); i++) {
		textures[meshs[i].MaterialIndex];
		uint startIndex = meshs[i].StartIndex;
		uint size = meshs[i].Size;
		glDrawArrays(GL_TRIANGLES, startIndex, size);
	}
}

uint SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


uint SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


uint SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkinnedMesh::ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
	string nodeName(pNode->mName.data);

	const aiAnimation* pAnimation = scene->mAnimations[0];

	glm::mat4 nodeTransformation = ConverAssimpMatrix(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D scaling;
		CalcInterpolatedScaling(scaling, animationTime, pNodeAnim);
		glm::mat4 scaleM = glm::scale(glm::mat4(1), glm::vec3(scaling.x, scaling.y, scaling.z));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion rotationQ;
		CalcInterpolatedRotation(rotationQ, animationTime, pNodeAnim);
		glm::mat4 rotationM = ConverAssimpMatrix(rotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D translation;
		CalcInterpolatedPosition(translation, animationTime, pNodeAnim);
		glm::mat4 translationM = glm::translate(glm::mat4(1), glm::vec3(translation.x, translation.y, translation.z));

		// Combine the above transformations
		nodeTransformation = translationM * rotationM * scaleM;
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransformation;

	if (boneMapping.find(nodeName) != boneMapping.end()) {
		uint boneIndex = boneMapping[nodeName];
		boneInfos[boneIndex].FinalTransform = globalInverseTransform * globalTransformation * boneInfos[boneIndex].BoneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(animationTime, pNode->mChildren[i], globalTransformation);
	}
}


void SkinnedMesh::BoneTransform(float deltaTime, vector<glm::mat4>& transforms)
{
	timer += deltaTime;
	glm::mat4 identity(1);

	if (!scene->HasAnimations())
		return;

	float TicksPerSecond = (float)(scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = timer * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)scene->mAnimations[0]->mDuration);

	//MessageBox(appMain.hWnd, L"prepare read node heirarchy", L"Information", MB_ICONINFORMATION);

	ReadNodeHeirarchy(AnimationTime, scene->mRootNode, identity);

	transforms.resize(numBones);

	for (uint i = 0; i < numBones; i++) {
		transforms[i] = boneInfos[i].FinalTransform;
	}
}


const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

glm::mat4 SkinnedMesh::ConverAssimpMatrix(const aiMatrix4x4& aiMatrix) {
	glm::mat4 conver(aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4,
		aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4,
		aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4,
		aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4);
	return conver;
}

glm::mat4 SkinnedMesh::ConverAssimpMatrix(const aiMatrix3x3& aiMatrix) {
	glm::mat4 conver(aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, 0,
		aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, 0,
		aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, 0,
		0, 0, 0, 0);
	return conver;
}