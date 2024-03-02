#include "CTVolume.h"
#include <mmOpenGLErrorCheck.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

CTVolume::CTVolume(std::string headerFilename, std::string imageFilename) :
	//m_Context(new QOpenGLContext),
	//m_SurfaceFormat(format),
	m_Initalized(false)
{
	/// Testing::: Initializing a texture outside of a SlicerWidget OpenGL Context
	//QSurfaceFormat SurfaceFormat;

	//SurfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
	//SurfaceFormat.setVersion(4, 6);
	//SurfaceFormat.setProfile(QSurfaceFormat::CoreProfile);

	//m_Surface->setFormat(SurfaceFormat);
	//m_Surface->create();

	//m_Context->setFormat(m_Surface->format());
	//m_Context->create();

	//m_Context = QOpenGLContext::currentContext();

	//std::cout << m_Context->format().majorVersion() << m_Context->format().minorVersion() << std::endl;

	//m_Context.setFormat(format);
	//m_Context.create();
	//m_Surface = m_Context.surface();
	//
	//std::cout << m_Context.format().majorVersion() << m_Context.format().minorVersion() << std::endl;

	readFromFile(headerFilename, imageFilename);
}

CTVolume::CTVolume()
{
	
}

CTVolume::~CTVolume()
{
	//TODO: Needs an OPENGL context to call this destructor
	deleteTexture();
	//freeVolume();
	//freeFlat(); // TODO: -> Delete scalar exception (Double deletion of m_pData? - understand copy constructors)
}


void CTVolume::readFromFile(std::string headerFilename, std::string imageFilename)
{
	m_sHeaderFilename = headerFilename;
	m_sImageFilename = imageFilename;
	m_vData.clear();

	readHeader();
	readImage();
	//init3DTexture();
}

int CTVolume::getNumRows()
{
	return m_nRows;
}

int CTVolume::getNumColumns()
{
	return m_nColumns;
}

int CTVolume::getNumSlices()
{
	return m_nSlices;
}

int CTVolume::getGrayMin()
{
	return m_nGrayMin;
}

int CTVolume::getGrayMax()
{
	return m_nGrayMax;
}

float CTVolume::getXDelta()
{
	return m_fXDelta;
}

float CTVolume::getYDelta()
{
	return m_fYDelta;
}

float CTVolume::getZDelta()
{
	return m_fZDelta;
}

glm::vec3 CTVolume::getVolumePhysicalDims()
{
	return glm::vec3(m_fXDelta*m_nRows,m_fYDelta*m_nColumns,m_fZDelta*m_nSlices);
}

std::vector<std::vector<std::vector<float>>> CTVolume::getVolumeData()
{
	//float*** volumeData = createVolume();
	std::vector<std::vector<std::vector<float>>> volumeData(m_nRows, std::vector<std::vector<float>>(m_nColumns, std::vector<float>(m_nSlices)));
		
	// TODO: This is very slow
	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j<m_nColumns; j++)
			for (int k = 0; k < m_nSlices; k++)
			{
				volumeData[i][j][k] = m_vData[i + m_nColumns * j + m_nSliceSize * k];
			}
		
	return volumeData;
}

std::vector<float> CTVolume::getFlatData()
{
	return m_vData;
}

float CTVolume::getVoxel(int x, int y, int z)
{
	if (isInBounds(x, y, z))
	{
		return m_vData.at(x + m_nColumns * y + m_nSliceSize * z);
	}
	else
	{
		return -1;
	}	
}

float CTVolume::getAxialAspectRatio()
{
	return ((m_nColumns * m_fXDelta) / (m_nRows * m_fYDelta));
}

float CTVolume::getCoronalAspectRatio()
{
	return ((m_nColumns * m_fXDelta) / (m_nSlices*m_fZDelta));
}

float CTVolume::getSagittalAspectRatio()
{
	return ((m_nRows*m_fXDelta) / (m_nSlices*m_fZDelta));
}

bool CTVolume::isInBounds(const int& x, const int& y, const int& z)
{
	if ((x >= 0) && (x < m_nRows) && (y >= 0) && (x < m_nRows) && (z >= 0) && (z < m_nSlices))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

GLuint CTVolume::getTextureID()
{
	return m_nCTTextureID;
}

void CTVolume::readHeader()
{
	// File Read in
	FILE *pFile;
	pFile = fopen(m_sHeaderFilename.c_str(),"rb"); // b for reading binary

	// Supporting variables
	int base = 0;			// where in the file to load the header
	unsigned char buf[4];	// buffer for storing when reading file
	int tmpInt;
	float *pTmpFloat;

	// Get the bits-per-pixel
	fseek(pFile, 0x48 + base, SEEK_SET);
	fread(buf, 2, 1, pFile);
	m_nDepth = (buf[0] << 8) | buf[1];

	// Get the dimensions of this image
	fseek(pFile, 42 + base, SEEK_SET);
	fread(buf, 2, 1, pFile);
	m_nColumns = (buf[0] << 8) | buf[1];
	fseek(pFile, 44 + base, SEEK_SET);
	fread(buf, 2, 1, pFile);
	m_nRows = (buf[0] << 8) | buf[1];
	fseek(pFile, 46 + base, SEEK_SET);
	fread(buf, 2, 1, pFile);
	m_nSlices = (buf[0] << 8) | buf[1];

	// Get the pixel dimensions: x-,y-, z-Delta
	fseek(pFile, 80 + base, SEEK_SET);
	fread(buf, 4, 1, pFile);
	tmpInt = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	pTmpFloat = (float*)(&tmpInt);
	m_fXDelta = *(pTmpFloat);
	fseek(pFile, 84 + base, SEEK_SET);
	fread(buf, 4, 1, pFile);
	tmpInt = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	pTmpFloat = (float*)(&tmpInt);
	m_fYDelta = *(pTmpFloat);
	fseek(pFile, 88 + base, SEEK_SET);
	fread(buf, 4, 1, pFile);
	tmpInt = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	pTmpFloat = (float*)(&tmpInt);
	m_fZDelta = *(pTmpFloat);

	// Get Minimum and Maximum gray values
	fseek(pFile, 140 + base, SEEK_SET);
	fread(buf, 4, 1, pFile);
	m_nGrayMax = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	fseek(pFile, 144 + base, SEEK_SET);
	fread(buf, 4, 1, pFile);
	m_nGrayMin = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

	m_nSliceSize = m_nRows * m_nColumns;
	m_nNumVoxels = m_nRows * m_nColumns * m_nSlices;
	m_bValid = 1;

	//calcMinDelta();

	fclose(pFile);
}

void CTVolume::readImage()
{
	FILE *pFile;
	pFile = fopen(m_sImageFilename.c_str(), "rb");

	// Create an array from header information
	initDataArray();

	// Read image and load data into a 1D array
	// TODO: Flexibility for data types. If voxel is only 1 byte, the data is twice as big as it needs to be.
	//			* Better memory management
	//unsigned char* byteBuffer;				// Grab 1 byte at a time from the image file
	std::vector<unsigned char> byteBuffer;		// Grab 1 byte at a time from the image file 
	float* pData = &m_vData[0];					// Pointer to base of Data Vector for writing 
	unsigned char* pByteBuffer;					// Pointer to base of Byte Buffer for reading

	switch (m_nDepth) {
	case 8:
		//byteBuffer = new unsigned char[m_nNumVoxels * 1];
		byteBuffer.resize(m_nNumVoxels);

		pByteBuffer = &byteBuffer[0];

		fread(&byteBuffer[0], 1, m_nNumVoxels, pFile);

		for (int i = 0; i < m_nNumVoxels; i++)
		{
			//m_vData[i] = static_cast<int>(byteBuffer[i]);
			pData[i] = static_cast<float>(pByteBuffer[i]);
		}

		//delete[] byteBuffer;
		//byteBuffer = nullptr;
		break;
	case 16:
		short tempVoxel;
		//byteBuffer = new unsigned char[m_nNumVoxels * 2];
		byteBuffer.resize(m_nNumVoxels * 2);

		pByteBuffer = &byteBuffer[0];

		fread(&byteBuffer[0], 2, m_nNumVoxels, pFile);

		for (int i = 0; i < m_nNumVoxels; ++i)
		{
			//tempVoxel = (byteBuffer[i * 2] << 8) | byteBuffer[i * 2 + 1];
			tempVoxel = (pByteBuffer[i * 2] << 8) | pByteBuffer[i * 2 + 1];
			//m_vData[i] = static_cast<float>(tempVoxel);
			//m_pData[i] = static_cast<float>(tempVoxel);
			pData[i] = static_cast<float>(tempVoxel);
		}

		//delete[] byteBuffer;
		//byteBuffer = nullptr;
		break;
	case 32:
		qDebug() << "No Support for 4byte voxels. ";
		break;
	case 64:
		qDebug() << "No Support for 8byte voxels. ";
		break;
	default:
		qDebug() << "Bit depth for Voxels are not 8, 16, 32, or 64. ";
		break;
	}
	fclose(pFile);
}

void CTVolume::initDataArray()
{
	//m_pData = new float[m_nRows * m_nColumns * m_nSlices];
	m_vData.resize(m_nNumVoxels);	
}

void CTVolume::init3DTexture()
{
	// TODO: How to avoid needing this initialization (don't quite understand how to create this without needing the QOpenGLWidget context)
	//				- I tried creating another context in the constructor
	if (m_Initalized) // return if already initialized by another shader
	{
		return;
	}
	else // Create texture in current (SlicerWidget) OpenGL Context
	{ 
		mmOpenGLClearErrorMacro();

		// Initialize 3D texture from CT Data
		glGenTextures(1, &m_nCTTextureID);
		glBindTexture(GL_TEXTURE_3D, m_nCTTextureID);
		// TODO: Figure out how to keep CTFlatData as short instead of float. When setting internal format to GL_R32F from GL_RED and GL_SHORT, imageLoad in fragment shader only reads normalized values [0-1.0f]
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, m_nColumns, m_nRows, m_nSlices);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_nColumns, m_nRows, m_nSlices, GL_RED, GL_FLOAT, &m_vData[0]);

		mmOpenGLCheckErrorMacro("Error with initializing CT Volume texture - OpenGL");

		m_Initalized = true;
	}

}

void CTVolume::bind3DImageTexture(GLuint binding)
{
	//m_Context->makeCurrent(m_Surface);
	m_nBinding = binding;
	glBindImageTexture(m_nBinding, m_nCTTextureID, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
	//m_Context->doneCurrent();
}

void CTVolume::unbind3DImageTexture()
{
	//m_Context->makeCurrent(m_Surface);
	glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
	//m_Context->doneCurrent();
}

//void CTVolume::calcMinDelta()
//{
//	m_fMinDelta = m_fZDelta;
//	
//	if (m_fYDelta < m_fMinDelta) 
//	{ 
//		m_fMinDelta = m_fYDelta; 
//	}
//	if (m_fXDelta < m_fMinDelta) 
//	{ 
//		m_fMinDelta = m_fXDelta; 
//	}		
//}

//float*** CTVolume::createVolume()
//{
//	// Create volume such that it is Rows(i) by Columns(j) by Slice(k)
//	// To Do: Add memory flexibility because voxel depth could be 8 or 16 bits.
//
//	float*** volumeData;
//
//	volumeData = new float**[m_nRows];
//	for (int i = 0; i < m_nRows; i++)
//	{
//		volumeData[i] = new float*[m_nColumns];
//		for (int j = 0; j < m_nColumns; j++)
//		{
//			volumeData[i][j] = new float[m_nSlices];
//			for (int k = 0; k < m_nSlices; k++)
//			{
//				volumeData[i][j][k] = 0;
//			}
//		}		
//	}
//	return volumeData;
//}

//void CTVolume::freeVolume(float*** volumeData)
//{
//	for (int i = 0; i < m_nRows; i++)
//	{
//		for (int j = 0; j < m_nColumns; j++)
//		{
//			delete[] volumeData[i][j];
//			volumeData[i][j] = nullptr;
//		}
//		delete[] volumeData[i];
//		volumeData[i] = nullptr;
//	}
//	delete[] volumeData;
//	volumeData = nullptr;
//}

//void CTVolume::freeFlat()
//{
//	//delete[] m_pData;
//	//m_pData = nullptr;
//}

void CTVolume::deleteTexture()
{
	//m_Context->makeCurrent(m_Surface);
	glDeleteTextures(1, &m_nCTTextureID);
	//m_nCTTextureID = 0;
	//m_Context->doneCurrent();
}

