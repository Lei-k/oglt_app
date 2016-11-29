#pragma once

#include "std_util.h"
#include "oglt_texture.h"
#include <glm/glm.hpp>

namespace oglt {
	enum MaterialParam {
		AMBIENT_COLOR, DIFFUSE_COLOR, SPECULAR_COLOR, EMISSIVE_COLOR,
		TRANSPARENCY_FACTORY, SHININESS_FACTORY, REFLECTION_FACTORY
	};

	class Material {
	public:
		Material();
		~Material();

		void setParam(MaterialParam param, glm::vec3 color);
		void setParam(MaterialParam param, float factory);

		glm::vec3* getColorParam(MaterialParam param);
		float getFactoryParam(MaterialParam param);
	private:
		glm::vec3 ambientColor;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
		glm::vec3 emissiveColor;
		float transparencyFactory;
		float shininessFactory;
		float reflectionFactory;

		bool enableAmbientColor;
		bool enableDiffuseColor;
		bool enableSpecularColor;
		bool enableEmissiveColor;
		bool enableTransparencyFactory;
		bool enableShininessFactory;
		bool enableReflectionFactory;

		vector<Texture> textures;
	};
}