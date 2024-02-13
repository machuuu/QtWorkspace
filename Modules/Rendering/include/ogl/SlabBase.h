#ifndef SLABBASE_H
#define SLABBASE_H

#include "SlicerBase.h"

// This Oblique Slicer class is derived from the SlicerBase Class (Used for Axial, Sagittal, Coronal Slicers)
// Oblique allows the camera to view the CT data at arbitrary angles
// Create this class within the Oblique Slicer Widget
// It contains the required common methods
class SlabBase : public SlicerBase
{
/* ENUMS */
public:

private:

/* METHODS */
public:
	SlabBase();
	SlabBase(int slicerXDim, int slicerYDim, int sliceZDim, int slicerWidth, int slicerHeight, CTVolume* CTData);
	~SlabBase();

	void draw(SlicerSelect select) override;

	//void initializeTextures() override;
	void computeSliceTexture(SlicerSelect select) override;


private:

/* VARIABLES */
public:
private:
	int m_ZVoxels;

};



#endif // SLABBASE_H