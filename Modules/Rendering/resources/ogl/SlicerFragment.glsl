#version 460 core

out vec4 FragColor;

in vec2 vTexCoord;

layout (binding = 0) uniform highp sampler2D sliceTexture;
layout (binding = 1) uniform highp sampler1D colorMap;

uniform float windowLevel;
uniform float windowWidth;

float CTWindow (float windowValue)
{
	float windowLow = windowLevel - windowWidth/2;
	float windowHigh = windowLevel + windowWidth/2;
	
	if ( windowValue < windowLow )
	{
		return 0.0;
	}
	else if ( windowValue > windowHigh )
	{
		return 1.0;
	}
	else
	{
		return (windowValue - windowLow)/(windowHigh - windowLow);
	}

}

void main()
{
	float sliceValue = CTWindow(texture(sliceTexture, vTexCoord).r);
	vec4 sliceColor = texture(colorMap,sliceValue);
	FragColor = vec4(sliceColor.rbg,1.0);
	//FragColor = vec4(sliceValue,sliceValue,sliceValue,1.0);
}