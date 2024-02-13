#ifndef SLICERHELPER_H
#define SLICERHELPER_H

#include <glad/glad.h>
#include "mmOpenGLErrorCheck.h"
#include <string>

namespace mm
{
	enum ColormapSelect {
		NEAREST,
		LINEAR
	};

	enum TextureFlipSelect {
		NONE,
		VERTICALLY
	};

	enum WindowType
	{
		LUNG,
		MEDIASTINUM,
		XSECTION
	};


	void getTextureQuad(GLuint &vertexArray, GLuint &vertexBuffer, GLuint &indexBuffer, TextureFlipSelect select);
	void drawTextureQuad(const GLuint &vertexArray, const GLuint &vertexBuffer, const GLuint &indexBuffer, int vertexCount = 6);
	GLuint getBlankTexture(int xDim, int yDim, int numChannels);
	GLuint getGrayColorMap(ColormapSelect select);
	GLuint getShaderProgram(const char* vertexSource, const char* fragmentSource);
	GLuint getComputeShaderProgram(const char* computeSource);
	GLuint compileShader(GLuint type, const char* shaderPath);
	void getWindowParameters(float &windowLevel, float &windowWidth, WindowType select);
	//void sliceSelect();
	void checkCompileErrors(GLuint shader, std::string type);
	
	
	
	void deleteTexture(GLuint textureID);


	
	//createTextureGeometry();
	//
	//createShaderProgram(std::string vertexSource, std::string fragmentSource, GLuint &programID);
	//createShaderProgram(std::string computeSource, GLuint programID);
	//
	//
	//getGrayScaleColorMap();
}

	

#endif // CROSSHAIR_H


//class SlicerHelper
//{
//	// Methods
//public:
//	SlicerHelper();
//	~SlicerHelper();
//
//	createBlankTexture();
//	createTextureGeometry();
//
//	createShaderProgram(std::string vertexSource, std::string fragmentSource, GLuint &programID);
//	createShaderProgram(std::string computeSource, GLuint programID);
//
//
//	getGrayScaleColorMap();
//
//private:
//
//	// Variables
//public:
//
//private:
//};