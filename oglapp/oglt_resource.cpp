#include "oglt_resource.h"

using namespace oglt;

vector<ShaderProgram> Resource::shaderPrograms;
vector<Texture> Resource::textures;
vector<Material> Resource::materials;

uint Resource::addShaderProgram(ShaderProgram & shaderProgram)
{
	// the shader program id store in Resource class
	uint shaderProgramId = OGLT_INVALID_SHADER_ID;
	FOR(i, ESZ(shaderPrograms)) {
		if (shaderProgram.getProgramID() == shaderPrograms[i].getProgramID()) {
			shaderProgramId = i;
			break;
		}
	}
	if (shaderProgramId == OGLT_INVALID_SHADER_ID) {
		shaderPrograms.push_back(shaderProgram);
		shaderProgramId = ESZ(shaderPrograms) - 1;
	}

	return shaderProgramId;
}

ShaderProgram * Resource::getShaderProgram(uint shaderId)
{
	if (shaderId == OGLT_INVALID_SHADER_ID || shaderId >= ESZ(shaderPrograms)) {
		// if use invalid shader programId set to default shader program id
		shaderId = DEFAULT_SHADER_PROGRAM_ID;
	}
	return &shaderPrograms[shaderId];
}

uint Resource::addTexture(Texture & texture)
{
	uint textureId = OGLT_INVALID_TEXTURE_ID;
	FOR(i, ESZ(textures)) {
		if (texture.getPath() == textures[i].getPath()) {
			textureId = i;
			break;
		}
	}
	if (textureId == OGLT_INVALID_TEXTURE_ID) {
		textures.push_back(texture);
		textureId = ESZ(textures) - 1;
	}
	return textureId;
}

Texture * Resource::getTexture(uint textureId)
{
	if (textureId == OGLT_INVALID_TEXTURE_ID || textureId >= ESZ(textures)) {
		textureId = DEFAULT_TEXTURE_ID;
	}
	return &textures[textureId];
}

uint Resource::addMaterial(Material & material)
{
	uint materialId = OGLT_INVALID_MATERIAL_ID;
	FOR(i, ESZ(materials)) {
		if (&material == &materials[i]) {
			materialId = i;
		}
	}
	if (materialId == OGLT_INVALID_MATERIAL_ID) {
		materials.push_back(material);
		materialId = ESZ(materials) - 1;
	}
	return materialId;
}

Material * Resource::getMaterial(uint materialId)
{
	return nullptr;
}
