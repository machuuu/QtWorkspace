

#include "mmUtilityTools.h"
#include "Logger.h"
#include <vector>
#include <iostream>
#include <fstream>

#include <sstream>

namespace mm
{
	void getTextureQuad(GLuint &vertexArray, GLuint &vertexBuffer, GLuint &indexBuffer, TextureFlipSelect select)
	{
		std::vector<GLfloat> vertices = {
			// positions			// texture coords (reversed)
			1.0f,  1.0f,  0.0f,    1.0f, 1.0f, // top right
			1.0f, -1.0f,  0.0f,    1.0f, 0.0f, // bottom right
			-1.0f, -1.0f,  0.0f,   0.0f, 0.0f, // bottom left
			-1.0f,  1.0f,  0.0f,   0.0f, 1.0f  // top left 
		};

		std::vector<GLuint> indices = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};
		
		switch (select)
		{
		case NONE:

			mmOpenGLClearErrorMacro();

			glGenVertexArrays(1, &vertexArray);
			glBindVertexArray(vertexArray);

			glGenBuffers(1, &vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

			// Position Attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));
			glEnableVertexAttribArray(0);
			// Texture Attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			glGenBuffers(1, &indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

			mmOpenGLCheckErrorMacro("Error with initializing texture geometry - OpenGL");
			break;

		case VERTICALLY:

			// flip vertically
			for (int i = 0; i < vertices.size(); i++)
			{
				if ((i + 1) % 5 == 0)
				{
					vertices[i] = 1.0f - vertices[i];
				}
			}

			mmOpenGLClearErrorMacro();

			glGenVertexArrays(1, &vertexArray);
			glBindVertexArray(vertexArray);

			glGenBuffers(1, &vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

			// Position Attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));
			glEnableVertexAttribArray(0);
			// Texture Attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			glGenBuffers(1, &indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

			mmOpenGLCheckErrorMacro("Error with initializing texture geometry - OpenGL");
			break;
		}
	}

	void drawTextureQuad(const GLuint &vertexArray, const GLuint &vertexBuffer, const GLuint &indexBuffer, int vertexCount /*= 6*/)
	{
		mmOpenGLClearErrorMacro();

		// Binding the Vertex Array for drawing airway tree
		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);

		mmOpenGLCheckErrorMacro("Error with drawing texture geometry - OpenGL");
	}

	GLuint getBlankTexture(int xDim, int yDim, int numChannels)
	{
		int totalSize = xDim * yDim * numChannels;

		std::vector<unsigned char> blankTexture(totalSize,255);

		GLuint blankTextureID;
		
		mmOpenGLClearErrorMacro();
		glGenTextures(1, &blankTextureID);
		glBindTexture(GL_TEXTURE_2D, blankTextureID);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, xDim, yDim);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, xDim, yDim, GL_RED, GL_UNSIGNED_BYTE, &blankTexture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		mmOpenGLCheckErrorMacro("Error with initializing blank texture - OpenGL");

		return blankTextureID;
	}

	GLuint getGrayColorMap(ColormapSelect select)
	{
		GLuint colorMapID;

		float grayLevels;
		int numColorChannels;
		int totalSize;
		int colorIdx;

		std::vector<unsigned char> grayColorMap;
		unsigned char* pColorMap;

		switch(select)
		{
		case NEAREST:

			grayLevels = 2;
			numColorChannels = 4;
			totalSize = grayLevels * numColorChannels;

			grayColorMap.resize(totalSize);
			pColorMap = &grayColorMap[0];

			colorIdx = 0;
			for (int i = 0; i < grayColorMap.size(); i = i + numColorChannels)
			{
				for (int j = 0; j < numColorChannels; j++)
				{
					if (j == 0)			// RED
						pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
					else if (j == 1)	// GREEN
						pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
					else if (j == 2)	// BLUE
						pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
					else				// ALPHA
						pColorMap[i + j] = 255;
				}
				colorIdx++;
			}

			mmOpenGLClearErrorMacro();
			glGenTextures(1, &colorMapID);
			glBindTexture(GL_TEXTURE_1D, colorMapID);
			glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGBA8, grayLevels);
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, grayLevels, GL_RGBA, GL_UNSIGNED_BYTE, &grayColorMap[0]);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			mmOpenGLCheckErrorMacro("Error with initializing gray colormap texture - OpenGL");
			break;

		case LINEAR:

			grayLevels = 256;
			numColorChannels = 4;
			totalSize = grayLevels * numColorChannels;

			grayColorMap.resize(totalSize);
			pColorMap = &grayColorMap[0];

			colorIdx = 0;
			for (int i = 0; i < totalSize; i = i + numColorChannels)
			{
				for (int j = 0; j < numColorChannels; j++)
				{
					if (j == 0)			// RED
						pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
					else if (j == 1)	// GREEN
						pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
					else if (j == 2)	// BLUE
						pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
					else				// ALPHA
						pColorMap[i + j] = 255;
				}
				colorIdx++;
			}

			mmOpenGLClearErrorMacro();
			glGenTextures(1, &colorMapID);
			glBindTexture(GL_TEXTURE_1D, colorMapID);
			glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGBA8, grayLevels);
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, grayLevels, GL_RGBA, GL_UNSIGNED_BYTE, &grayColorMap[0]);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			mmOpenGLCheckErrorMacro("Error with initializing gray colormap texture - OpenGL");
			break;

		default:

			qDebug() << "Select interpolate or linear for creating gray color map";
			break;
		}
		

		return colorMapID;
	}

	GLuint getShaderProgram(const char* vertexSource, const char* fragmentSource)
	{
		mmOpenGLClearErrorMacro();
		
		GLuint vertex, fragment, program;
		vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
		checkCompileErrors(vertex, "VERTEX");
		fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
		checkCompileErrors(fragment, "FRAGMENT");		

		program = glCreateProgram();		
		glAttachShader(program, vertex);	
		glAttachShader(program, fragment);		
		glLinkProgram(program);		
		checkCompileErrors(program, "PROGRAM");		
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		mmOpenGLCheckErrorMacro("Error with compiling vertex and fragment program.");

		return program;
	}

	GLuint getComputeShaderProgram(const char* computeSource)
	{
		mmOpenGLClearErrorMacro();

		GLuint compute, program;
		compute = compileShader(GL_COMPUTE_SHADER, computeSource);
		checkCompileErrors(compute, "COMPUTE");

		program = glCreateProgram();
		glAttachShader(program, compute);
		glLinkProgram(program);
		checkCompileErrors(program, "PROGRAM");
		glDeleteShader(compute);

		mmOpenGLCheckErrorMacro("Error with compiling compute program.");

		return program;
	}

	GLuint compileShader(GLuint type, const char* shaderPath)
	{
		const char* shaderType;
		if (type == GL_VERTEX_SHADER)
			shaderType = "Vertex";
		else if (type == GL_FRAGMENT_SHADER)
			shaderType = "Fragment";
		else if (type == GL_COMPUTE_SHADER)
			shaderType = "Compute";
		else
			shaderType = "Unknown";

		std::string code;
		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			shaderFile.open(shaderPath);
			std::stringstream shaderStream;
			// read file's buffer contents into streams
			shaderStream << shaderFile.rdbuf();
			// close file handlers
			shaderFile.close();
			// convert stream into string
			code = shaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			qDebug() << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ::" << shaderType;
		}
		const char* shaderCode = code.c_str();
		unsigned int id = glCreateShader(type);
		glShaderSource(id, 1, &shaderCode, 0);
		glCompileShader(id);

		qDebug() << "COMPILED SHADER::" << shaderType;

		return id;
	}

	void getWindowParameters(float &windowLevel, float &windowWidth, WindowType select)
	{
		switch (select) {
		case LUNG:
			windowLevel = -350;
			windowWidth = 1800;
			break;
		case MEDIASTINUM:
			windowLevel = -40;
			windowWidth = 400;
			break;
		case XSECTION:
			windowLevel = -900;
			windowWidth = 600;
			break;
		default:
			windowLevel = 0;
			windowWidth = 2000;
			qDebug() << "Window Level: " << windowLevel << " Window Width: " << windowWidth;
		}
	}

	void checkCompileErrors(GLuint shader, std::string type)
	{		
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);				
				qDebug() << "ERROR::SHADER_COMPILATION_ERROR of type:" << type.c_str() << "\n" << infoLog << "\n ------------------------------------------------------ ";
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				qDebug() << "ERROR::PROGRAM_LINKING_ERROR of type: " << type.c_str() << "\n" << infoLog << "\n ------------------------------------------------------ ";
			}
		}
	}

	void deleteTexture(GLuint textureID)
	{
		glDeleteTextures(1, &textureID);
	}

}