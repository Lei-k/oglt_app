#pragma once

#include "std_util.h"
#include "oglt_texture.h"
#include <glm/glm.hpp>

namespace oglt {
	enum MaterialParam {
		AMBIENT_COLOR, DIFFUSE_COLOR, SPECULAR_COLOR, EMISSIVE_COLOR,
		TRANSPARENCY_Factor, SHININESS_Factor, REFLECTION_Factor
	};

	class Material {
	public:
		Material();
		~Material();

		void setParam(MaterialParam param, glm::vec3 color);
		void setParam(MaterialParam param, float Factor);

		glm::vec3* getColorParam(MaterialParam param);
		float getFactorParam(MaterialParam param);
		void addTexture(Texture& texture);
	private:
		glm::vec3 ambientColor;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
		glm::vec3 emissiveColor;
		float transparencyFactor;
		float shininessFactor;
		float reflectionFactor;

		bool enableAmbientColor;
		bool enableDiffuseColor;
		bool enableSpecularColor;
		bool enableEmissiveColor;
		bool enableTransparencyFactor;
		bool enableShininessFactor;
		bool enableReflectionFactor;

		vector<Texture> textures;
	};
}