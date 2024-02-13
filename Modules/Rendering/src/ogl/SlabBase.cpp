#include "SlabBase.h"


SlabBase::SlabBase(int slicerXDim, int slicerYDim, int slicerZDim, int slicerWidth, int slicerHeight, CTVolume *CTData) :
	m_ZVoxels(slicerZDim),
	SlicerBase(slicerXDim, slicerYDim, slicerWidth, slicerHeight, CTData)
{
	//setWindowParameters(MEDIASTINUM);
	//
	//m_Camera.SetMouseMovementSpeed(0.0025f);
	//
	//m_Scale = glm::mat4(1.0f);
	//m_Model = glm::mat4(1.0f);
	//m_View = glm::mat4(1.0f);
	//m_Projection = glm::mat4(1.0f);
	//m_OrthoProjPos = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
}

SlabBase::SlabBase()
{

}

SlabBase::~SlabBase()
{
	//glDeleteTextures(1, &m_SliceTexture);
	//glDeleteTextures(1, &m_ColormapTexture);
	//glDeleteProgram(m_ShaderProgram);
	//glDeleteProgram(m_ComputeProgram);
	//glDeleteVertexArrays(1, &m_VertexArray);
	//glDeleteBuffers(1, &m_VertexBuffer);
	//glDeleteBuffers(1, &m_IndexBuffer);
}

void SlabBase::draw(SlicerSelect select)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	computeSliceTexture(select);
	drawSliceTexture();
	drawCrosshair();

	std::cout << m_SliceSelect << std::endl;
}

void SlabBase::computeSliceTexture(SlicerSelect select)
{
	int fieldOfView = 25;
	int depthOfVision = m_CTData->getNumSlices();
	
	//m_CTData->bind3DImageTexture(0);
	mmOpenGLClearErrorMacro();
	glBindImageTexture(0, m_CTTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
	glBindImageTexture(1, m_SliceTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glUseProgram(m_ComputeProgram);
	glUniform1i(glGetUniformLocation(m_ComputeProgram, "sliceSelect"), m_SliceSelect);
	glUniform1i(glGetUniformLocation(m_ComputeProgram, "fieldOfView"), fieldOfView);
	glUniform1i(glGetUniformLocation(m_ComputeProgram, "depthOfVision"), depthOfVision);
	switch (select)
	{
	case AXIAL:
		glDispatchCompute(m_XVoxels, m_YVoxels, 1);
		break;
	default:
		break;
	}
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glUseProgram(0);
	mmOpenGLCheckErrorMacro("Error with Selecting Slice - SlicerBase");
}

//void ObliqueSlicerBase::initializeTextures()
//{
//	createTextureQuad(VERTICALLY);
//	createCTVolumeTexture();
//	createSliceTexture();
//	createColorMap(GRAYSCALE);
//}
//
//void ObliqueSlicerBase::computeSliceTexture(SlicerComputeSelect select)
//{
//	//m_CTData->bind3DImageTexture(0);
//	mmOpenGLClearErrorMacro();
//	glBindImageTexture(0, m_CTTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
//	glBindImageTexture(1, m_SliceTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
//	glUseProgram(m_ComputeProgram);
//	glUniform1i(glGetUniformLocation(m_ComputeProgram, "sliceSelect"), m_SliceSelect);
//	switch (select)
//	{
//	case AXIAL:
//		glDispatchCompute(m_XVoxels, m_YVoxels, 1);
//		break;
//	case SAGITTAL:
//		glDispatchCompute(1, m_XVoxels, m_YVoxels);
//		break;
//	case CORONAL:
//		glDispatchCompute(m_XVoxels, 1, m_YVoxels);
//		break;
//	default:
//		break;
//	}
//	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
//	glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
//	glUseProgram(0);
//	mmOpenGLCheckErrorMacro("Error with Selecting Slice - SlicerBase");
//}
