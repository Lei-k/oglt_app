#pragma once

#include "oglt_irenderable.h"
#include "oglt_shader.h"
#include "oglt_vbo.h"
#include "oglt_texture.h"

namespace oglt {
	class Material
	{
	public:
		int iTexture;
	};

	class AssimpModel : public IRenderable
	{
	public:
		bool loadModelFromFile(char* sFilePath);

		static void finalizeVBO();
		static void bindModelsVAO();

		virtual void render(int renderType = OGLT_RENDER_SELF);
		AssimpModel();
	private:
		bool loaded;
		static VertexBufferObject vboData;
		static UINT vao;
		static vector<Texture> textures;
		vector<uint> meshStartIndices;
		vector<uint> meshSizes;
		vector<uint> materialIndices;
		int numMaterials;
	};
}
