#ifndef CTVOLUME_H
#define CTVOLUME_H

#include <string>
#include <glad/glad.h>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOffscreenSurface>
#include <glm/glm.hpp>

#include <vector>

class CTVolume
{
public:
	CTVolume();
	CTVolume(std::string headerFilename, std::string imageFilename);
	~CTVolume();

	void readFromFile(std::string headerFilename, std::string imageFilename);
		
	// Get Functions
	int getNumRows();
	int getNumColumns();
	int getNumSlices();
	int getGrayMin();
	int getGrayMax();
	float getXDelta();
	float getYDelta();
	float getZDelta();
	glm::vec3 getVolumePhysicalDims();
	std::vector<std::vector<std::vector<float>>> getVolumeData();
	std::vector<float> getFlatData();
	float getVoxel(int x, int y, int z);
	//float* getFlatData();
	float getAxialAspectRatio();
	float getCoronalAspectRatio();
	float getSagittalAspectRatio();

	/* OpenGL members */
	GLuint getTextureID();
	void init3DTexture();
	void deleteTexture();
	void bind3DImageTexture(GLuint binding);
	void unbind3DImageTexture();

	// Delete the copy constructor/assignment
	// https://www.khronos.org/opengl/wiki/Common_Mistakes#Texture_upload_and_pixel_reads
	//CTVolume(const CTVolume &) = delete;
	//CTVolume& operator=(const CTVolume &) = delete;
	//CTVolume(CTVolume &&other) : m_nCTTextureID(other.m_nCTTextureID)
	//{
	//	other.m_nCTTextureID = 0;
	//}
	//CTVolume& operator=(CTVolume &&other)
	//{
	//	if (this != &other)
	//	{
	//		deleteTexture();
	//		std::swap(m_nCTTextureID, other.m_nCTTextureID);
	//	}
	//}


private:
	void readHeader();
	void readImage();

	void initDataArray();

	//void calcMinDelta();

	//float*** createVolume();

	//void freeVolume(float*** volumeData);
	//void freeFlat();
	

public:

private:
	std::string m_sHeaderFilename;
	std::string m_sImageFilename;
	
	// Image/Volume information from the header file
	int m_nDepth;
	int m_nColumns;
	int m_nRows;
	int m_nSlices;
	int m_nSliceSize;
	int m_nNumVoxels;
	float m_fXDelta;
	float m_fYDelta;
	float m_fZDelta;
	float m_fMinDelta;
	int m_nGrayMax;
	int m_nGrayMin;

	// OpenGL Members
	GLuint m_nCTTextureID = 0;
	GLuint m_nBinding;
	QOpenGLContext m_Context;
	QSurfaceFormat m_SurfaceFormat;
	QSurface *m_Surface;
	//QOffscreenSurface *m_Surface;
	
	bool m_bValid; // if valid image
	bool m_Initalized;

	// Volume data
	// TO DO: Add flexibility such that volume can be declared as short, int, char, float etc...
	//			* better memory management
	std::vector<float> m_vData;
	//float* m_pData;
};
#endif // CTVOLUME_H