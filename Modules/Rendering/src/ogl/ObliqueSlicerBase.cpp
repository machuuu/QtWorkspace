#include "ObliqueSlicerBase.h"

ObliqueSlicerBase::ObliqueSlicerBase(int slicerXDim, int slicerYDim, int slicerZDim, int slicerWidth, int slicerHeight, CTVolume *CTData)
{
	initialize(slicerXDim, slicerYDim, slicerZDim, slicerWidth, slicerHeight, CTData);
}

ObliqueSlicerBase::ObliqueSlicerBase()
{

}

ObliqueSlicerBase::~ObliqueSlicerBase()
{
	//glDeleteTextures(1, &m_SliceTexture);
	//glDeleteTextures(1, &m_ColormapTexture);
	//glDeleteProgram(m_ShaderProgram);
	//glDeleteProgram(m_ComputeProgram);
	//glDeleteVertexArrays(1, &m_VertexArray);
	//glDeleteBuffers(1, &m_VertexBuffer);
	//glDeleteBuffers(1, &m_IndexBuffer);
}

void ObliqueSlicerBase::initialize(int slicerXDim, int slicerYDim, int sliceZDim, int slicerWidth, int slicerHeight, CTVolume* CTData)
{
	SlicerBase::initialize(slicerXDim, slicerYDim, slicerWidth, slicerHeight, CTData);
	
	m_ZVoxels = sliceZDim;
	m_TextureWidth = 1024;
	m_TextureHeight = 1024;
	
	m_Camera.SetModelOffset(glm::vec3(m_XVoxels / 2.0, m_YVoxels / 2.0, m_ZVoxels / 2.0));
	m_Camera.SetKeyboardMovementSpeed(25.0f);
	m_SlicerBounds = glm::ivec2(m_TextureWidth, m_TextureHeight);
	m_CTVolumeBounds = glm::ivec3(m_XVoxels, m_YVoxels, m_ZVoxels);

	m_SlicerVoxelInMillimeters = glm::vec2(m_CTData->getXDelta(), m_CTData->getYDelta());
	m_CTVolumeVoxelInMillimeters = glm::vec3(m_CTData->getXDelta(), m_CTData->getYDelta(), m_CTData->getZDelta());

	m_AspectRatio = glm::vec3(m_CTData->getVolumePhysicalDims().x / m_CTData->getVolumePhysicalDims().x, m_CTData->getVolumePhysicalDims().y / m_CTData->getVolumePhysicalDims().x, m_CTData->getVolumePhysicalDims().z / m_CTData->getVolumePhysicalDims().x);
}

void ObliqueSlicerBase::draw(SlicerSelect select)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	computeSliceTexture(select);
	drawSliceTexture();
	drawCrosshair();

	std::cout << m_SliceSelect << std::endl;
}

void ObliqueSlicerBase::computeSliceTexture(SlicerSelect select)
{
	int fieldOfView = 25;
	int depthOfVision = m_CTData->getNumSlices();

	//m_CTData->bind3DImageTexture(0);
	mmOpenGLClearErrorMacro();
	glBindImageTexture(0, m_CTTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
	glBindImageTexture(1, m_SliceTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glUseProgram(m_ComputeProgram);	

	//m_Camera.ProcessMouseRotation(1.0f, 1.0f, false);

	glm::vec3 pos = m_Camera.GetModelOffset();
	glm::mat3 rot = m_Camera.GetRotationMatrix();

	glm::vec3 aspect = getAspectRatioVector();

	//glm::mat3 textureScale = glm::mat3(1.0f);
	//updateAspectRatioVector();
	//textureScale[0][0] = m_AspectRatio.x / m_AspectRatio.x;
	//textureScale[1][1] = (m_AspectRatio.y / m_AspectRatio.x);
	//textureScale[2][2] = (m_AspectRatio.z / m_AspectRatio.x);

	glUniform3fv(glGetUniformLocation(m_ComputeProgram, "CTAspectRatio"), 1, &m_AspectRatio[0]);
	glUniformMatrix3fv(glGetUniformLocation(m_ComputeProgram, "rotationMatrix"), 1, GL_FALSE, &rot[0][0]);
	glUniform3fv(glGetUniformLocation(m_ComputeProgram, "positionVector"), 1, &pos[0]);

	glUniform2iv(glGetUniformLocation(m_ComputeProgram, "sliceBounds"), 1, &m_SlicerBounds[0]);
	glUniform3iv(glGetUniformLocation(m_ComputeProgram, "volumeBounds"), 1, &m_CTVolumeBounds[0]);
	glUniform2fv(glGetUniformLocation(m_ComputeProgram, "textureResolution"), 1, &m_SlicerVoxelInMillimeters[0]);
	glUniform3fv(glGetUniformLocation(m_ComputeProgram, "volumeResolution"), 1, &m_CTVolumeVoxelInMillimeters[0]);
	
	//glUniformMatrix4fv(glGetUniformLocation(m_ComputeProgram, "view"), 1, GL_FALSE, &view[0][0]);



	//glUniform2fv(glGetUniformLocation(m_ComputeProgram, "textureCenter"),2,&m_TextureCenter[0]);	



	//glUniform1i(glGetUniformLocation(m_ComputeProgram, "cameraX"), 0);
	//glUniform1i(glGetUniformLocation(m_ComputeProgram, "cameraY"), 0);
	//glUniform1i(glGetUniformLocation(m_ComputeProgram, "cameraZ"), 0);

	//g//lUniform1f(glGetUniformLocation(m_ComputeProgram, "Camera"));

	//glUniform3fv(glGetUniformLocation(m_ComputeProgram, "cameraPosition"), 1, &cameraPos[0]);

	//glUniform4fv(glGetUniformLocation(m_ComputeProgram, "cameraPos"), 4, &cameraPos[0]); 
	//glUniform1i(glGetUniformLocation(m_ComputeProgram, "outputSliceWidth"), m_TextureWidth);
	//glUniform1i(glGetUniformLocation(m_ComputeProgram, "outputSliceHeight"), m_TextureHeight);

	switch (select)
	{
	case OBLIQUE:
		glDispatchCompute(32, 32, 1); 	
		break;
	default:
		break;
	}
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glUseProgram(0);
	mmOpenGLCheckErrorMacro("Error with Selecting Slice - ObliqueSlicerBase");
}

void ObliqueSlicerBase::createSliceTexture()
{
	int numChannels = 1;

	// Allow the oblique slicer to host a drawing surface
	int totalSize = m_TextureWidth * m_TextureHeight * numChannels;

	// Set all values to 255 and use only 1 channel (RED)
	std::vector<unsigned char> blankTexture(totalSize, 255);

	mmOpenGLClearErrorMacro();
	glGenTextures(1, &m_SliceTexture);
	glBindTexture(GL_TEXTURE_2D, m_SliceTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, m_TextureWidth, m_TextureHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_TextureWidth, m_TextureHeight, GL_RED, GL_UNSIGNED_BYTE, &blankTexture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mmOpenGLCheckErrorMacro("Error with initializing blank texture - OpenGL");
}

void ObliqueSlicerBase::setOrbitCenter(int xVox, int yVox, int zVox)
{
	m_Camera.SetModelOffset(glm::vec3(xVox, yVox, zVox));
}

void ObliqueSlicerBase::getOrbitCenter(int& xVox, int& yVox, int& zVox)
{
	auto offset = m_Camera.GetModelOffset();
	xVox = round(offset.x);
	yVox = round(offset.y);
	zVox = round(offset.z);
}

void ObliqueSlicerBase::mouseOnMove(QVector2D mousePosition)
{
	float xoffset = mousePosition.x() - m_MouseLastPosition.x();
	float yoffset = mousePosition.y() - m_MouseLastPosition.y();

	if (m_MouseRightClick)
	{
		//computeSlicesToSend(mousePosition);
		updateCrosshairPosition(mousePosition);
		m_Camera.ProcessMouseRotation(xoffset,yoffset,m_ShiftSelect);
		// Do nothing if not left click
	}
	else
	{
		// do nothing
	}
}

void ObliqueSlicerBase::keyOnPress(int keySelect)
{
	float inc = 5;
	
	switch (keySelect)
	{
	case Qt::Key_Shift:
		m_ShiftSelect = true;
		break;
	case Qt::Key_W:
		m_Camera.ProcessKeyboardTranslation(SlicerCamera::UP);
		break;
	case Qt::Key_A:
		m_Camera.ProcessKeyboardTranslation(SlicerCamera::LEFT);
		break;
	case Qt::Key_S:
		m_Camera.ProcessKeyboardTranslation(SlicerCamera::DOWN);
		break;
	case Qt::Key_D:
		m_Camera.ProcessKeyboardTranslation(SlicerCamera::RIGHT);
		break;
	case Qt::Key_Q:
		m_Camera.ProcessKeyboardTranslation(SlicerCamera::BACKWARD);
		break;
	case Qt::Key_E:
		m_Camera.ProcessKeyboardTranslation(SlicerCamera::FORWARD);
		break;
	case Qt::Key_F:
		m_Camera.UpRotation(-inc);
		break;
	case Qt::Key_G:
		m_Camera.UpRotation(inc);
		break;
	case Qt::Key_Y:
		m_Camera.FrontRotation(inc);
		break;
	case Qt::Key_H:
		m_Camera.FrontRotation(-inc);
		break;
	case Qt::Key_V:
		m_Camera.RightRotation(-inc);
		break;
	case Qt::Key_B:
		m_Camera.RightRotation(inc);
		break;
	case Qt::Key_T:
		m_ZoomAmount = 1.0f;
		m_Camera.SetDefault();
		m_Camera.SetModelOffset(glm::vec3(m_XVoxels / 2.0, m_YVoxels / 2.0, m_ZVoxels / 2.0));
		m_Camera.SetMouseMovementSpeed(m_ZoomSensitivity * m_ZoomAmount);
		m_Camera.SetKeyboardMovementSpeed(25.0f);
		m_OrthoProjPos = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
		updateCrosshairPosition(QVector2D(m_SlicerWidth / 2.0f, m_SlicerHeight / 2.0f));
		createProjectionMatrix();
		cursorOff();
		break;
	default:
		m_ShiftSelect = false;
		break;
	}
}

glm::vec3 ObliqueSlicerBase::getAspectRatioVector()
{
	glm::vec3 aspectRatio = m_Camera.GetRotationMatrix() * glm::normalize(glm::vec3(m_CTData->getVolumePhysicalDims().x, m_CTData->getVolumePhysicalDims().y, m_CTData->getVolumePhysicalDims().z));
	
	float maxValue = glm::max(aspectRatio.x, aspectRatio.y);
	maxValue = glm::max(maxValue, aspectRatio.z);

	aspectRatio = glm::vec3(abs(aspectRatio.x / maxValue), abs(aspectRatio.y / maxValue), abs(aspectRatio.z / maxValue));

	return aspectRatio;
}


