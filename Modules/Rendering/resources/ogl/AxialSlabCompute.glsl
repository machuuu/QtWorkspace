#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, r32f) uniform readonly highp image3D inputVolume;
layout (binding = 1, r32f) uniform writeonly highp image2D outputSlice;

uniform int sliceSelect;
uniform int fieldOfView;
uniform int depthOfVision;

// Influenced by the Depth-Weighting Max Procedure
vec4 calcSlabValue (int x, int y, int z)
{
	ivec3 volumePosition;
	vec4 voxelValue;
	float weightedVoxel;
	float weight;

	float maxValue; // Assuming that no voxel values are lower than -3000;
	float returnValue;
	
	// Find the Max Value within the Field of View
	for (int i = 0; i < fieldOfView; i++)
	{
		
		
		volumePosition = ivec3(x,y,z+i);
		voxelValue = imageLoad(inputVolume,volumePosition);
		weight = depthOfVision - (i - 1);
		weightedVoxel = voxelValue.r * weight;
		
		if ( i == 0 )
		{
			maxValue = weightedVoxel;
		}
		else if (voxelValue.r == 0.0)   // HACK -> If outside Volume (imageLoad reads 0) (Some Errors could occur if Voxel Value is indeed 0) This prevents the gray results at the end of the slicer
		{
			// ignore assigning maxValue;
		}
		else if (weightedVoxel >= maxValue)
		{
			maxValue = weightedVoxel;
		}
	}

	// Divide the Max Value by the Depth of Vision
	returnValue = maxValue / depthOfVision;

	return vec4(returnValue, 0.0, 0.0, 1.0);
}


void main()
{
	ivec2 imageBounds = imageSize(outputSlice);
	int x = int(gl_GlobalInvocationID.x);
	int y = int(gl_GlobalInvocationID.y);
	int z = sliceSelect;
	//int z = int(gl_GlobalInvocationID.z);
	

	if ( ((x >= imageBounds.x) || (y >= imageBounds.y)) )
	{
		return; // return;
	}

	imageStore(outputSlice,ivec2(x,y),calcSlabValue(x,y,z));

	//ivec3 volumePosition = ivec3(x,y,z);
	//vec4 volumeValue = imageLoad(inputVolume,volumePosition);
	//imageStore(outputSlice,ivec2(x,y),volumeValue);
}
