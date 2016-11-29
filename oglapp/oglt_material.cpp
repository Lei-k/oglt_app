#include "oglt_material.h"

using namespace oglt;

Material::Material()
{
	enableAmbientColor = false;
	enableDiffuseColor = false;
	enableSpecularColor = false;
	enableEmissiveColor = false;
	enableTransparencyFactor = false;
	enableShininessFactor = false;
	enableReflectionFactor = false;
}

Material::~Material()
{

}

void Material::setParam(MaterialParam param, glm::vec3 color)
{
	switch (param) {
	case AMBIENT_COLOR:
		ambientColor = color;
		enableAmbientColor = true;
		break;
	case DIFFUSE_COLOR:
		diffuseColor = color;
		enableDiffuseColor = true;
		break;
	case SPECULAR_COLOR:
		specularColor = color;
		enableSpecularColor = true;
		break;
	case EMISSIVE_COLOR:
		emissiveColor = color;
		enableEmissiveColor = true;
		break;
	default:
		fprintf(stderr, "Error: Use undefind color\n");
	}
}

void Material::setParam(MaterialParam param, float Factor)
{
	switch (param) {
	case TRANSPARENCY_Factor:
		transparencyFactor = Factor;
		enableTransparencyFactor = true;
		break;
	case SHININESS_Factor:
		shininessFactor = Factor;
		enableShininessFactor = true;
		break;
	case REFLECTION_Factor:
		reflectionFactor = Factor;
		enableReflectionFactor = true;
		break;
	default:
		fprintf(stderr, "Error: Use undefined Factor\n");
	}
}

glm::vec3 * oglt::Material::getColorParam(MaterialParam param)
{
	switch (param) {
	case AMBIENT_COLOR:
		if (enableAmbientColor)
			return &ambientColor;
		break;
	case DIFFUSE_COLOR:
		if (enableDiffuseColor)
			return &diffuseColor;
		break;
	case SPECULAR_COLOR:
		if (enableSpecularColor)
			return &specularColor;
		break;
	case EMISSIVE_COLOR:
		if (enableEmissiveColor)
			return &emissiveColor;
		break;
	default:
		fprintf(stderr, "Error: Use undefind color\n");
	}
	fprintf(stderr, "Error: Use unenable color\n");
	return nullptr;
}

float Material::getFactorParam(MaterialParam param)
{
	switch (param) {
	case TRANSPARENCY_Factor:
		if (enableTransparencyFactor)
			return transparencyFactor;
		break;
	case SHININESS_Factor:
		if (enableShininessFactor)
			return shininessFactor;
		break;
	case REFLECTION_Factor:
		if (enableReflectionFactor)
			return reflectionFactor;
		break;
	default:
		fprintf(stderr, "Error: Use undefined Factor\n");
	}
	fprintf(stderr, "Error: Use unenable Factor\n");
	return 0.0f;
}

void Material::addTexture(Texture& texture)
{
	textures.push_back(texture);
}

