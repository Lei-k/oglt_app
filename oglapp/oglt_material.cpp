#include "oglt_material.h"

using namespace oglt;

Material::Material()
{
	enableAmbientColor = false;
	enableDiffuseColor = false;
	enableSpecularColor = false;
	enableEmissiveColor = false;
	enableTransparencyFactory = false;
	enableShininessFactory = false;
	enableReflectionFactory = false;
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

void Material::setParam(MaterialParam param, float factory)
{
	switch (param) {
	case TRANSPARENCY_FACTORY:
		transparencyFactory = factory;
		enableTransparencyFactory = true;
		break;
	case SHININESS_FACTORY:
		shininessFactory = factory;
		enableShininessFactory = true;
		break;
	case REFLECTION_FACTORY:
		reflectionFactory = factory;
		enableReflectionFactory = true;
		break;
	default:
		fprintf(stderr, "Error: Use undefined factory\n");
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

float oglt::Material::getFactoryParam(MaterialParam param)
{
	switch (param) {
	case TRANSPARENCY_FACTORY:
		if (enableTransparencyFactory)
			return transparencyFactory;
		break;
	case SHININESS_FACTORY:
		if (enableShininessFactory)
			return shininessFactory;
		break;
	case REFLECTION_FACTORY:
		if (enableReflectionFactory)
			return reflectionFactory;
		break;
	default:
		fprintf(stderr, "Error: Use undefined factory\n");
	}
	fprintf(stderr, "Error: Use unenable factory\n");
	return 0.0f;
}

