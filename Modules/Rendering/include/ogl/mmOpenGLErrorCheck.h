#pragma once

#include <glad/glad.h>
#include "Logger.h"
#include <iostream>
#include <ostream>
#include <sstream>

inline const char *mmOpenGLStrError(GLenum code)
{
	switch (code)
	{
	case GL_NO_ERROR:
		return "No error";
	case GL_INVALID_ENUM:
		return "Invalid enum";
	case GL_INVALID_VALUE:
		return "Invalid value";
	case GL_INVALID_OPERATION:
		return "Invalid operation";
	case GL_OUT_OF_MEMORY:
		return "Out of memory";
	}
	return "Unknown error";
}

inline int mmGetOpenGLErrors(int maxNum, unsigned int *errCode, const char **errDesc)
{
	int i = 0;
	GLenum code = glGetError();
	if (i < maxNum)
	{
		errCode[i] = code;
		errDesc[i] = mmOpenGLStrError(code);
	}
	while (code != GL_NO_ERROR && i < maxNum)
	{
		i++;
		code = glGetError();
		if (i < maxNum)
		{
			errCode[i] = code;
			errDesc[i] = mmOpenGLStrError(code);
		}
	}
	return i;
}

inline void mmPrintOpenGLErrors(std::ostream &os, int maxErrors, int numErrors, unsigned int *errCode, const char **errDesc)
{
	os << numErrors << " OpenGL errors detected" << std::endl;
	for (int i = 0; (i < numErrors) && (i < maxErrors); ++i)
	{
		os << " " << i << " : (" << errCode[i] << ") " << errDesc[i] << std::endl;
	}
	if (numErrors > maxErrors)
	{
		os << "More than " << maxErrors << " detected! The remainder are not reported" << std::endl;
	}
}

inline void mmClearOpenGLErrors(const unsigned int maxErrors = 16)
{
	GLenum glError;
	unsigned int i = 0;
	do
	{
		glError = glGetError();
		++i;
	} while (i < maxErrors && glError != GL_NO_ERROR);
}

#define mmOpenGLClearErrorMacro() mmClearOpenGLErrors(16)
#define mmOpenGLCheckErrorMacro(message)									\
{																			\
	const int maxErrors = 16;												\
	unsigned int errCode[maxErrors] = { 0 };								\
	const char *errDesc[maxErrors] = { NULL };								\
																			\
	int numErrors = mmGetOpenGLErrors(maxErrors, errCode, errDesc);			\
																			\
	if (numErrors)															\
	{																		\
		std::ostringstream oss;												\
		oss << message << std::endl;										\
		mmPrintOpenGLErrors(oss, maxErrors, numErrors, errCode, errDesc);   \
		qDebug() << oss.str().c_str();										\
	}																		\
}																			\
