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

	//Load plugins from the executable directory (optional)
	FbxString path = FbxGetApplicationDirectory();
	manager->LoadPluginsDirectory(path.Buffer());
	
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

	FbxSystemUnit sceneSystemUnit = scene->GetGlobalSettings().GetSystemUnit();
	if (sceneSystemUnit.GetScaleFactor() != 1.0)
	{
		//The unit in this is centimeter.
		FbxSystemUnit::cm.ConvertScene(scene);
	}

	FbxGeometryConverter geoConverter(manager);
	
	if (!geoConverter.Triangulate(scene, true)) {
		fprintf(stderr, "Error: Unable conver to triangle mesh.\n");
	}

	importer->Destroy();
	importer = NULL;

	loadFromScene(scene);

	loaded = true;

	cout << "vertices size: " << vertices.getCurrentSize() / sizeof(glm::vec3) << endl;
	cout << "colors size: " << colors.getCurrentSize() / sizeof(glm::vec4) << endl;
	cout << "uv0s size: " << uvs[0].getCurrentSize() / sizeof(glm::vec2) << endl;
	cout << "normals size: " << normals.getCurrentSize() / sizeof(glm::vec3) << endl;
	cout << "mesh size: " << meshs.size() << endl;

	finalizeVBO();

	return true;
}

bool FbxModel::loadFromScene(FbxScene * scene)
{
	FbxNode* rootNode = scene->GetRootNode();

	if (rootNode == NULL) {
		return false;
	}

	vertices.createVBO();
	colors.createVBO();
	normals.createVBO();
	FOR(i, MAX_UV_CHANNEL) {
		uvs[i].createVBO();
	}

	processNode(rootNode);
}

void FbxModel::traverseNode(FbxNode * parentNode, vector<vec3>& vertices, vector<vec2>& uvs, vector<vec3>& normals)
{
	FOR(i, parentNode->GetChildCount()) {
		FbxNode* childNode = parentNode->GetChild(i);
		traverseNode(childNode, vertices, uvs, normals);
	}
}

void FbxModel::processNode(FbxNode * node)
{
	if (node->GetNodeAttribute()) {
		switch (node->GetNodeAttribute()->GetAttributeType()) {
		case FbxNodeAttribute::eMesh:
			processMesh(node);
			break;
		case FbxNodeAttribute::eSkeleton:
			break;
		case FbxNodeAttribute::eLight:
			break;
		case FbxNodeAttribute::eCamera:
			break;
		}
	}

	FOR(i, node->GetChildCount()) {
		processNode(node->GetChild(i));
	}
}

void FbxModel::processMesh(FbxNode * node)
{
	FbxMesh* mesh = node->GetMesh();
	if (mesh == NULL) {
		return;
	}

	glm::vec3 vertex[3];
	glm::vec4 color[3];
	glm::vec3 normal[3];
	glm::vec3 tangent[3];
	glm::vec2 uv[3][MAX_UV_CHANNEL];

	MeshEntry meshEntry;
	meshEntry.startIndex = vertices.getCurrentSize() / sizeof(glm::vec3);

	int vertexCounter = 0;

	FOR(i, mesh->GetPolygonCount()) {
		FOR (j, 3) {
			int ctrlPointIndex = mesh->GetPolygonVertex(i, j);

			readVertex(mesh, ctrlPointIndex, &vertex[j]);

			readColor(mesh, ctrlPointIndex, vertexCounter, &color[j]);

			FOR(k, mesh->GetTextureUVCount()) {
				if (k >= MAX_UV_CHANNEL)
					break;
				readUV(mesh, ctrlPointIndex, mesh->GetTextureUVIndex(i, j), k, &(uv[j][k]));
				uvs[k].addData(&uv[j][k], sizeof(glm::vec2));
			}

			readNormal(mesh, ctrlPointIndex, vertexCounter, &normal[j]);

			readTangent(mesh, ctrlPointIndex, vertexCounter, &tangent[j]);

			vertexCounter++;

			vertices.addData(&vertex[j], sizeof(glm::vec3));
			colors.addData(&color[j], sizeof(glm::vec4));
			normals.addData(&normal[j], sizeof(glm::vec3));
		}
	}
	meshEntry.size = vertices.getCurrentSize() / sizeof(glm::vec3) - meshEntry.startIndex;
	meshs.push_back(meshEntry);
}

void FbxModel::processSkeleton(FbxNode * node)
{
}

void FbxModel::processLight(FbxNode * node)
{
}

void FbxModel::processCamera(FbxNode * node)
{
}

void FbxModel::readVertex(FbxMesh * mesh, int ctrlPointIndex, glm::vec3 * outVertex)
{
	FbxVector4 ctrlPoint = mesh->GetControlPointAt(ctrlPointIndex);
	outVertex->x = ctrlPoint[0];
	outVertex->y = ctrlPoint[1];
	outVertex->z = ctrlPoint[2];
}

void FbxModel::readColor(FbxMesh * mesh, int ctrlPointIndex, int vertexCounter, glm::vec4 * outColor)
{
	if (mesh->GetElementVertexColorCount() < 1) {
		return;
	}

	FbxGeometryElementVertexColor* vertexColor = mesh->GetElementVertexColor(0);
	if (vertexColor == NULL) {
		outColor->x = 1.0f;
		outColor->y = 1.0f;
		outColor->z = 1.0f;
		outColor->w = 1.0f;
		return;
	}

	switch (vertexColor->GetMappingMode()) {
	case FbxGeometryElement::eByControlPoint:
		switch (vertexColor->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
			outColor->x = vertexColor->GetDirectArray().GetAt(ctrlPointIndex).mRed;
			outColor->y = vertexColor->GetDirectArray().GetAt(ctrlPointIndex).mGreen;
			outColor->z = vertexColor->GetDirectArray().GetAt(ctrlPointIndex).mBlue;
			outColor->w = vertexColor->GetDirectArray().GetAt(ctrlPointIndex).mAlpha;
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = vertexColor->GetIndexArray().GetAt(ctrlPointIndex);
			outColor->x = vertexColor->GetDirectArray().GetAt(id).mRed;
			outColor->y = vertexColor->GetDirectArray().GetAt(id).mGreen;
			outColor->z = vertexColor->GetDirectArray().GetAt(id).mBlue;
			outColor->w = vertexColor->GetDirectArray().GetAt(id).mAlpha;
			break;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexColor->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
			outColor->x = vertexColor->GetDirectArray().GetAt(vertexCounter).mRed;
			outColor->y = vertexColor->GetDirectArray().GetAt(vertexCounter).mGreen;
			outColor->z = vertexColor->GetDirectArray().GetAt(vertexCounter).mBlue;
			outColor->w = vertexColor->GetDirectArray().GetAt(vertexCounter).mAlpha;
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = vertexColor->GetIndexArray().GetAt(vertexCounter);
			outColor->x = vertexColor->GetDirectArray().GetAt(id).mRed;
			outColor->y = vertexColor->GetDirectArray().GetAt(id).mGreen;
			outColor->z = vertexColor->GetDirectArray().GetAt(id).mBlue;
			outColor->w = vertexColor->GetDirectArray().GetAt(id).mAlpha;
			break;
		}
		break;
	}
	
}

void FbxModel::readUV(FbxMesh * mesh, int ctrlPointIndex, int textureUVIndex, int uvLayer, glm::vec2 * outUV)
{
	if (uvLayer >= mesh->GetTextureUVCount())
		return;

	FbxGeometryElementUV* vertexUV = mesh->GetElementUV(uvLayer);
	if (vertexUV == NULL) {
		return;
	}

	switch (vertexUV->GetMappingMode()) {
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
			outUV->x = vertexUV->GetDirectArray().GetAt(ctrlPointIndex)[0];
			outUV->x = vertexUV->GetDirectArray().GetAt(ctrlPointIndex)[1];
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = vertexUV->GetIndexArray().GetAt(ctrlPointIndex);
			outUV->x = vertexUV->GetDirectArray().GetAt(id)[0];
			outUV->x = vertexUV->GetDirectArray().GetAt(id)[1];
			break;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
			outUV->x = vertexUV->GetDirectArray().GetAt(textureUVIndex)[0];
			outUV->y = vertexUV->GetDirectArray().GetAt(textureUVIndex)[1];
			break;
		}
		break;
	}
}

void FbxModel::readNormal(FbxMesh * mesh, int ctrlPointIndex, int vertexCounter, glm::vec3 * outNormal)
{
	if (mesh->GetElementNormalCount() < 1)
		return;

	FbxGeometryElementNormal* normal = mesh->GetElementNormal(0);
	if (normal == NULL)
		return;
	
	switch (normal->GetMappingMode()) {
	case FbxGeometryElement::eByControlPoint:
		switch (normal->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
			outNormal->x = normal->GetDirectArray().GetAt(ctrlPointIndex)[0];
			outNormal->y = normal->GetDirectArray().GetAt(ctrlPointIndex)[1];
			outNormal->z = normal->GetDirectArray().GetAt(ctrlPointIndex)[2];
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = normal->GetIndexArray().GetAt(ctrlPointIndex);
			outNormal->x = normal->GetDirectArray().GetAt(id)[0];
			outNormal->y = normal->GetDirectArray().GetAt(id)[1];
			outNormal->z = normal->GetDirectArray().GetAt(id)[2];
			break;
		}

	case FbxGeometryElement::eByPolygonVertex:
		switch (normal->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
			outNormal->x = normal->GetDirectArray().GetAt(vertexCounter)[0];
			outNormal->y = normal->GetDirectArray().GetAt(vertexCounter)[1];
			outNormal->z = normal->GetDirectArray().GetAt(vertexCounter)[2];
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = normal->GetIndexArray().GetAt(vertexCounter);
			outNormal->x = normal->GetDirectArray().GetAt(id)[0];
			outNormal->y = normal->GetDirectArray().GetAt(id)[1];
			outNormal->z = normal->GetDirectArray().GetAt(id)[2];
			break;
		}
		break;
	}
}

void FbxModel::readTangent(FbxMesh * mesh, int ctrlPointIndex, int vertexCounter, glm::vec3 * outTangent)
{
	if (mesh->GetElementTangentCount() < 1)
		return;

	FbxGeometryElementTangent* tangent = mesh->GetElementTangent(0);
	if (tangent == NULL)
		return;

	switch (tangent->GetMappingMode()) {
	case FbxGeometryElement::eByControlPoint:
		switch (tangent->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
			outTangent->x = tangent->GetDirectArray().GetAt(ctrlPointIndex)[0];
			outTangent->y = tangent->GetDirectArray().GetAt(ctrlPointIndex)[1];
			outTangent->z = tangent->GetDirectArray().GetAt(ctrlPointIndex)[2];
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = tangent->GetIndexArray().GetAt(ctrlPointIndex);
			outTangent->x = tangent->GetDirectArray().GetAt(id)[0];
			outTangent->y = tangent->GetDirectArray().GetAt(id)[1];
			outTangent->z = tangent->GetDirectArray().GetAt(id)[2];
			break;
		}
	case FbxGeometryElement::eByPolygonVertex:
		switch (tangent->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
			outTangent->x = tangent->GetDirectArray().GetAt(vertexCounter)[0];
			outTangent->y = tangent->GetDirectArray().GetAt(vertexCounter)[1];
			outTangent->z = tangent->GetDirectArray().GetAt(vertexCounter)[2];
			break;
		case FbxGeometryElement::eIndexToDirect:
			int id = tangent->GetIndexArray().GetAt(vertexCounter);
			outTangent->x = tangent->GetDirectArray().GetAt(id)[0];
			outTangent->y = tangent->GetDirectArray().GetAt(id)[1];
			outTangent->z = tangent->GetDirectArray().GetAt(id)[2];
			break;
		}
		break;
	}
}

void FbxModel::finalizeVBO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertices
	vertices.bindVBO();
	vertices.uploadDataToGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// colors
	colors.bindVBO();
	colors.uploadDataToGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// normals
	normals.bindVBO();
	normals.uploadDataToGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// uvs
	uvs[0].bindVBO();
	uvs[0].uploadDataToGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void FbxModel::render(int renderType)
{
	if (!loaded)
		return;

	glBindVertexArray(vao);
	FOR(i, ESZ(meshs)) {
		glDrawArrays(GL_TRIANGLES, meshs[i].startIndex, meshs[i].size);
	}
	
}
