#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, r32f) uniform readonly highp image3D inputVolume;
layout (binding = 1, r32f) uniform writeonly highp image2D outputSlice;

uniform int sliceSelect;

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

	ivec3 volumePosition = ivec3(x,y,z);
	vec4 volumeValue = imageLoad(inputVolume,volumePosition);
	imageStore(outputSlice,ivec2(x,y),volumeValue);

//	if ( (x < imageBounds.x) && (y < imageBounds.y) && (z == sliceSelect)) 
//	{
//		ivec3 volumePosition = ivec3(x,y,z);
//		vec4 volumeValue = imageLoad(inputVolume,volumePosition);
//		imageStore(outputSlice,ivec2(x,y),volumeValue);
//	}
}
