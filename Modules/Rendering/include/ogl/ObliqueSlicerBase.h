#ifndef OBLIQUESLICERBASE_H
#define OBLIQUESLICERBASE_H

#include "SlicerBase.h"

class ObliqueSlicerBase : public SlicerBase
{
	/* ENUMS */
public:

private:

	/* METHODS */
public:
	ObliqueSlicerBase();
	ObliqueSlicerBase(int slicerXDim, int slicerYDim, int sliceZDim, int slicerWidth, int slicerHeight, CTVolume* CTData);
	~ObliqueSlicerBase();

	ObliqueSlicerBase(const ObliqueSlicerBase&) = delete;
	ObliqueSlicerBase(ObliqueSlicerBase&&) = delete;
	ObliqueSlicerBase& operator=(const ObliqueSlicerBase&) = delete;
	ObliqueSlicerBase& operator=(ObliqueSlicerBase&&) = delete;

	void initialize(int slicerXDim, int slicerYDim, int sliceZDim, int slicerWidth, int slicerHeight, CTVolume* CTData);

	void draw(SlicerSelect select) override;

	//void initializeTextures() override;
	void computeSliceTexture(SlicerSelect select) override;

	void createSliceTexture() override;

	void mouseOnMove(QVector2D mousePosition) override;
	void keyOnPress(int keySelect) override;
	glm::vec3 getAspectRatioVector();
private:

	/* VARIABLES */
public:
private:
	int m_ZVoxels;
	int m_TextureWidth;
	int m_TextureHeight;
	glm::vec2 m_TextureCenter;
	glm::vec3 m_AspectRatio;

	glm::ivec2 m_SlicerBounds;
	glm::ivec3 m_CTVolumeBounds;
	glm::vec2 m_SlicerVoxelInMillimeters;
	glm::vec3 m_CTVolumeVoxelInMillimeters;
};
#endif // OBLIQUESLICERBASE