#pragma once

#include "std_util.h"
#include "oglt_util.h"

enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};

/********************************

Class:		Texture

Purpose:	Wraps OpenGL texture
object and performs
their loading.

********************************/
namespace oglt {
	class Texture
	{
	public:
		void createEmptyTexture(int a_iWidth, int a_iHeight, GLenum format);
		void createFromData(GLubyte* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps = false);

		bool reloadTexture();

		bool loadTexture2D(string a_sPath, bool bGenerateMipMaps = false);
		void bindTexture(int iTextureUnit = 0);

		void setFiltering(int a_tfMagnification, int a_tfMinification);

		void setSamplerParameter(GLenum parameter, GLenum value);

		int getMinificationFilter();
		int getMagnificationFilter();

		int getWidth();
		int getHeight();
		int getBPP();

		uint getTextureID();

		string getPath();

		void deleteTexture();

		Texture();
	private:

		int width, height, iBPP; // Texture width, height, and bytes per pixel
		uint textureId; // Texture name
		uint samplerId; // Sampler name
		bool bMipMapsGenerated;

		int tfMinification, tfMagnification;

		string sPath;
	};
}