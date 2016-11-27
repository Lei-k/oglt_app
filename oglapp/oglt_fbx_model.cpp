#include "oglt_fbx_model.h"

#include <fbxsdk\fileio\fbxiosettings.h>

using namespace oglt;
using namespace glm;

FbxManager* FbxModel::manager;

FbxModel::FbxModel()
{
	loaded = false;
}

FbxModel::~FbxModel()
{

}

bool FbxModel::initialize()
{
	manager = FbxManager::Create();
	if (manager == NULL) {
		fprintf(stderr, "Error: Unable to create Fbx Manager!!!\n");
		return false;
	}
	else {
		cout << "Autodesk FBX SDK version " << manager->GetVersion() << endl;
	}

	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);

	return true;
}

void FbxModel::destroyManager()
{
	manager->Destroy();
}

bool FbxModel::load(const string & fileName)
{
	if (manager == NULL) {
		fprintf(stderr, "Error: The Fbx Manager is not initialze.\n");
		return false;
	}

	// Create the importer.
	int fileFormat = -1;
	importer = FbxImporter::Create(manager, "My Importer");
	if (manager->GetIOPluginRegistry()->DetectReaderFileFormat(fileName.c_str(), fileFormat))
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		fileFormat = manager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
	}

	// Initialize the importer by providing a filename.
	if (importer->Initialize(fileName.c_str(), fileFormat) == true)
	{
		cout << "Importing file " << fileName << endl;
		cout << "Please wait!" << endl;
	}
	else
	{
		fprintf(stderr, "Unable to open file %s \n Error reported: %s\n",
			fileName, importer->GetStatus().GetErrorString());
		return false;
	}

	scene = FbxScene::Create(manager, "My Scene");
	if (scene == NULL)
	{
		fprintf(stderr, "Error: Unable to create FBX scene!\n");
		return false;
	}

	if (!importer->Import(scene)) {
		fprintf(stderr, "Error: Unable importer to scene.\n");
		return false;
	}
	importer->Destroy();
	importer = NULL;

	loadFromScene(scene);

	return true;
}

bool FbxModel::loadFromScene(FbxScene * scene)
{
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	FbxNode* rootNode = scene->GetRootNode();

	if (rootNode == NULL) {
		return false;
	}

	traverseNode(rootNode, vertices, uvs, normals);
}

void FbxModel::traverseNode(FbxNode * parentNode, vector<vec3>& vertices, vector<vec2>& vus, vector<vec3>& normals)
{
	FOR(i, parentNode->GetChildCount()) {
		FbxNode* childNode = parentNode->GetChild(i);
		if (childNode == NULL) continue;
		FbxMesh* mesh = childNode->GetMesh();
		if (mesh == NULL) continue;
		FOR(j, mesh->GetControlPointsCount()) {
			FbxVector4 coord = mesh->GetControlPointAt(j);
			vec3 vertex; vertex[0] = coord[0];
			vertex[1] = coord[1]; vertex[2] = coord[2];
			vertices.push_back(vertex);
		}

		
	}
}

void FbxModel::render(int renderType)
{

}
