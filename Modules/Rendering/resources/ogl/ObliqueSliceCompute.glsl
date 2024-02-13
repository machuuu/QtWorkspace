#version 460 core

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout (binding = 0, r32f) uniform readonly highp image3D inputVolume;
layout (binding = 1, r32f) uniform writeonly highp image2D outputSlice;

//uniform mat4 view;
//uniform vec2 textureCenter;
//uniform vec4 cameraPos;

uniform vec3 positionVector;
uniform mat3 rotationMatrix;
uniform vec3 CTAspectRatio;
uniform ivec2 sliceBounds;
uniform ivec3 volumeBounds;
uniform vec2 textureResolution;
uniform vec3 volumeResolution;

//uniform int outputSliceHeight;
//uniform int outputSliceWidth;

// The global invocation treats the top left corner as (0,0) while the texture treats bottom left as (0,0)
// The texture's origin is then set to the middle of the texture. This is so that the center of the texture 
//		is always at the voxel the camera is at in the 3D Volume.
ivec2 setGlobalInvocationToCenteredTextureCoord (ivec2 globalInvocationLocation, vec2 textureDim)
{
	int xLoc = (globalInvocationLocation.x) - int(round(float(textureDim.x)/2.0));
	int yLoc = (globalInvocationLocation.y) - int(round(float(textureDim.x)/2.0));

	return ivec2(xLoc, yLoc);
}

// Get the voxel that is part of the slice of the 3D volume determined by the camera's position and normal vector
vec4 voxelFrom3DVolume (ivec2 textLoc, vec3 volumeBounds)
{
	// pad zero to represent texture location in 3D
	// find position in the texture in terms of physical dimensions
	vec3 textLoc3D = vec3(textLoc.xy, 0.0);
	vec3 textLoc3DPhysical = vec3(textureResolution.xy, 0.0) * textLoc3D;
	
	// apply the translation and rotation matrix of the camera to texture slice
	// rotate the centered texture, move to camera position
	vec3 posInVolumePhysical = (rotationMatrix * textLoc3DPhysical) + (volumeResolution * positionVector);

	// get the voxel location from physical location
	vec3 posInVolume = vec3(posInVolumePhysical.x/volumeResolution.x,posInVolumePhysical.y/volumeResolution.y,posInVolumePhysical.z/volumeResolution.z);	
	
	// position in texture should be an integer for loading a voxel
	ivec3 roundedPosInVolume = ivec3(round(posInVolume)); 

	// get voxel value for the corresponding location in the slice of the 3D volume
	vec4 voxelValue = imageLoad(inputVolume,roundedPosInVolume);

	return voxelValue;
}

void main()
{	
	ivec2 globalInvo = ivec2(gl_GlobalInvocationID.xy);
	ivec2 textureLoc = setGlobalInvocationToCenteredTextureCoord(ivec2(gl_GlobalInvocationID.xy), sliceBounds);

	//
	
	imageStore(outputSlice,globalInvo,voxelFrom3DVolume(textureLoc, volumeBounds));
}
