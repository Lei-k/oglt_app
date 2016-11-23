#pragma once
#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "oglt_shader.h"
#include "oglt_texture.h"
#include "oglt_vbo.h"

namespace oglt {
	/********************************

	Class:		FreeTypeFont

	Purpose:	Wraps FreeType fonts and
	their usage with OpenGL.

	********************************/

	class FreeTypeFont
	{
	public:
		bool loadFont(string sFile, int iPXSize);
		bool loadSystemFont(string sName, int iPXSize);

		int getTextWidth(string sText, int iPXSize);

		void print(string sText, int x, int y, int iPXSize = -1);
		void printFormatted(int x, int y, int iPXSize, char* sText, ...);

		void deleteFont();

		void setShaderProgram(ShaderProgram* a_shShaderProgram);

		FreeTypeFont();
	private:
		void createChar(int iIndex);

		Texture tCharTextures[256];
		int iAdvX[256], iAdvY[256];
		int iBearingX[256], iBearingY[256];
		int iCharWidth[256], iCharHeight[256];
		int iLoadedPixelSize, iNewLine;

		bool loaded;

		uint vao;
		VertexBufferObject vboData;

		FT_Library ftLib;
		FT_Face ftFace;
		ShaderProgram* shaderProgram;
	};
}
