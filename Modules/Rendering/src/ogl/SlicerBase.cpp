#include "SlicerBase.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "rendering.configure.h"

SlicerBase::SlicerBase(int slicerXDim, int slicerYDim, int slicerWidth, int slicerHeight, CTVolume *CTData) :
	m_XVoxels(slicerXDim),
	m_YVoxels(slicerYDim),
	m_SlicerWidth(slicerWidth),
	m_SlicerHeight(slicerHeight),
	m_CTData(CTData),
	m_ZoomAmount(1.0f),
	m_SliceSelect(0),
	m_ZoomSensitivity(0.01f),
	m_MouseLeftClick(false),
	m_MouseRightClick(false),
	m_ShiftSelect(false),
	m_CursorOn(false)
{
	setWindowParameters(LUNG);
	
	m_Camera.SetMouseMovementSpeed(0.0025f);
	
	m_TextureScale = glm::mat2(1.0f);
	m_ModelScale = glm::mat4(1.0f);
	m_Model = glm::mat4(1.0f);
	m_View = glm::mat4(1.0f);
	m_Projection = glm::mat4(1.0f);
	m_OrthoProjPos = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
}

//SlicerBase::SlicerBase(int slicerWidth, int slicerHeight, CTVolume *CTData) :
//	m_SlicerWidth(slicerWidth),
//	m_SlicerHeight(slicerHeight),
//	m_CTData(CTData),
//	m_ZoomAmount(1.0f),
//	m_SliceSelect(0),
//	m_ZoomSensitivity(0.01f),
//	m_MouseLeftClick(false),
//	m_MouseRightClick(false),
//	m_CursorOn(false)
//{
//	setWindowParameters(LUNG);
//
//	m_Camera.SetMouseMovementSpeed(0.0025f);
//
//	m_Scale = glm::mat4(1.0f);
//	m_Model = glm::mat4(1.0f);
//	m_View = glm::mat4(1.0f);
//	m_Projection = glm::mat4(1.0f);
//	m_OrthoProjPos = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
//}

SlicerBase::SlicerBase()
{

}

SlicerBase::~SlicerBase()
{
	glDeleteTextures(1, &m_SliceTexture);
	glDeleteTextures(1, &m_ColormapTexture);
	glDeleteProgram(m_ShaderProgram);
	glDeleteProgram(m_ComputeProgram);
	glDeleteVertexArrays(1, &m_VertexArray);
	glDeleteBuffers(1, &m_VertexBuffer);
	glDeleteBuffers(1, &m_IndexBuffer);
}

void SlicerBase::initializeTextures()
{
	createTextureQuad(VERTICALLY);
	createCTVolumeTexture();
	createSliceTexture();
	createColorMap(GRAYSCALE);
}

void SlicerBase::initializeShader(const char* vertexPath, const char* fragmentPath)
{
	createShaderProgram(vertexPath, fragmentPath);
}

void SlicerBase::initializeComputeShader(const char* computePath)
{
	createComputeShaderProgram(computePath);
}

void SlicerBase::initializeCrosshair()
{
	m_Crosshair.init(glm::vec2(0.0f, 0.0f));
}

void SlicerBase::updateSlicerDimensions(int width, int height)
{
	m_SlicerWidth = width;
	m_SlicerHeight = height;
}

void SlicerBase::updateSliceSelect(int sliceSelect)
{
	m_SliceSelect = sliceSelect;
}

void SlicerBase::updateProjectionMatrix(ProjectionUpdateSelect select)
{
	float xCenter;
	float yCenter;

	switch (select)
	{
	case fromZoom:

		// Calculate center of orthogonal projection position
		xCenter = (m_OrthoProjPos[0] + m_OrthoProjPos[1]) / 2;
		yCenter = (m_OrthoProjPos[2] + m_OrthoProjPos[3]) / 2;

		// Shift to 0,0
		m_OrthoProjPos[0] -= xCenter;
		m_OrthoProjPos[1] -= xCenter;
		m_OrthoProjPos[2] -= yCenter;
		m_OrthoProjPos[3] -= yCenter;
		// Scale with zoom
		m_OrthoProjPos[0] = -m_ZoomAmount;
		m_OrthoProjPos[1] = m_ZoomAmount;
		m_OrthoProjPos[2] = -m_ZoomAmount;
		m_OrthoProjPos[3] = m_ZoomAmount;
		// Move back to location
		m_OrthoProjPos[0] += xCenter;
		m_OrthoProjPos[1] += xCenter;
		m_OrthoProjPos[2] += yCenter;
		m_OrthoProjPos[3] += yCenter;

		break;

	case fromPan:

		m_OrthoProjPos[0] = -1.0f * m_ZoomAmount - m_Camera.GetModelOffset().x;
		m_OrthoProjPos[1] = 1.0f * m_ZoomAmount - m_Camera.GetModelOffset().x;
		m_OrthoProjPos[2] = -1.0f * m_ZoomAmount + m_Camera.GetModelOffset().y;
		m_OrthoProjPos[3] = 1.0f * m_ZoomAmount + m_Camera.GetModelOffset().y;

		break;

	default:
		break;
	}

	createProjectionMatrix();
}

void SlicerBase::updateCrosshairPosition(QVector2D mouseLocation)
{
	m_Crosshair.updatePosition(mouseLocation, m_SlicerWidth, m_SlicerHeight);
}

void SlicerBase::mouseOnClick(QVector2D mousePosition, Qt::MouseButton mouseButton)
{
	m_MouseLastPosition = mousePosition;

	switch (mouseButton)
	{
	case Qt::LeftButton:
		m_MouseLeftClick = true;
		break;
	case Qt::RightButton:
		m_MouseRightClick = true;
		updateCrosshairPosition(m_MouseLastPosition);
		computeSlicesToSend(m_MouseLastPosition);
		break;
	default:
		m_MouseLeftClick = false;
		m_MouseRightClick = false;
		break;
	}
}

void SlicerBase::mouseOnMove(QVector2D mousePosition)
{
	if (m_MouseLeftClick)
	{
		float xoffset = mousePosition.x() - m_MouseLastPosition.x();
		float yoffset = mousePosition.y() - m_MouseLastPosition.y();

		m_Camera.ProcessMouseTranslation(xoffset, yoffset, false);
		m_MouseLastPosition = mousePosition;

		updateProjectionMatrix(fromPan);
		
	}
	else if (m_MouseRightClick)
	{
		computeSlicesToSend(mousePosition);
		updateCrosshairPosition(mousePosition);
		// Do nothing if not left click
	}
	else
	{
		// do nothing
	}
}

void SlicerBase::mouseOnRelease()
{
	m_MouseLeftClick = false;
	m_MouseRightClick = false;
}

void SlicerBase::mouseOnWheel(int wheelDelta)
{
	// Scroll up zoom inwards, Scroll down zoom outwards
	// Divide by 120 to get each scroll click to be 1.0f and adjust using 0.01f sensitivity
	m_ZoomAmount += (wheelDelta / 120.0f) * m_ZoomSensitivity;

	if (m_ZoomAmount > 5.0f)
		m_ZoomAmount = 5.0f;

	if (m_ZoomAmount < m_ZoomSensitivity)	// Avoid 0.0f -> Set to smallest unit of zoom
		m_ZoomAmount = m_ZoomSensitivity;

	m_Camera.SetMouseMovementSpeed(m_ZoomAmount * m_ZoomSensitivity);

	updateProjectionMatrix(fromZoom);
}

void SlicerBase::keyOnPress(int keySelect)
{
	switch (keySelect)
	{
	case Qt::Key_Shift:
		m_ShiftSelect = true;
	case Qt::Key_T:
		m_ZoomAmount = 1.0f;
		m_Camera.SetModelOffset(glm::vec3(0.0f, 0.0f, 0.0f));
		m_Camera.SetMouseMovementSpeed(m_ZoomSensitivity * m_ZoomAmount);
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

void SlicerBase::keyOnRelease(int keySelect)
{
	switch (keySelect)
	{
	case Qt::Key_Shift:
		m_ShiftSelect = false;
		break;
	default:
		break;
	}
}

void SlicerBase::draw(SlicerSelect select)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	computeSliceTexture(select);
	drawSliceTexture();
	drawCrosshair();

	std::cout << m_SliceSelect << std::endl;
}

void SlicerBase::computeSliceTexture(SlicerSelect select)
{
	//m_CTData->bind3DImageTexture(0);
	mmOpenGLClearErrorMacro();
	glBindImageTexture(0, m_CTTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
	glBindImageTexture(1, m_SliceTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glUseProgram(m_ComputeProgram);
	glUniform1i(glGetUniformLocation(m_ComputeProgram, "sliceSelect"), m_SliceSelect);
	switch (select)
	{
	case AXIAL:
		glDispatchCompute(m_XVoxels, m_YVoxels, 1);
		break;
	case SAGITTAL:
		glDispatchCompute(1, m_XVoxels, m_YVoxels);
		break;
	case CORONAL:
		glDispatchCompute(m_XVoxels, 1, m_YVoxels);
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

void SlicerBase::drawSliceTexture()
{
	mmOpenGLClearErrorMacro();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_SliceTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, m_ColormapTexture);
	glUseProgram(m_ShaderProgram);
	m_TextureScale = glm::mat2(1.0f);
	m_ModelScale = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "model"), 1, GL_FALSE, &m_Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "modelScale"), 1, GL_FALSE, &m_ModelScale[0][0]);
	glUniformMatrix2fv(glGetUniformLocation(m_ShaderProgram, "textureScale"), 1, GL_FALSE, &m_TextureScale[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "view"), 1, GL_FALSE, &m_View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "projection"), 1, GL_FALSE, &m_Projection[0][0]);
	glUniform1f(glGetUniformLocation(m_ShaderProgram, "windowLevel"), m_WindowLevel);
	glUniform1f(glGetUniformLocation(m_ShaderProgram, "windowWidth"), m_WindowWidth);
	mmOpenGLCheckErrorMacro("Error with computing and painting texture - OPENGL");
	drawTextureQuad();
}

void SlicerBase::drawCrosshair()
{
	if (m_CursorOn)
	{
		m_Crosshair.draw();
	}	
}

void SlicerBase::cursorOn()
{
	m_CursorOn = true;
}

void SlicerBase::cursorOff()
{
	m_CursorOn = false;
}

void SlicerBase::setCameraVectors(glm::vec3 cameraFront, glm::vec3 cameraRight, glm::vec3 cameraUp)
{
	m_Camera.SetFront(cameraFront);
	m_Camera.SetRight(cameraRight);
	m_Camera.SetUp(cameraUp);
	m_Camera.CheckCameraVectors();
}

void SlicerBase::setWindowParameters(WindowType select)
{
	switch (select) {
	case LUNG:
		m_WindowLevel = -350;
		m_WindowWidth = 1800;
		break;
	case MEDIASTINUM:
		m_WindowLevel = -40;
		m_WindowWidth = 400;
		break;
	case XSECTION:
		m_WindowLevel = -900;
		m_WindowWidth = 600;
		break;
	default:
		m_WindowLevel = 0;
		m_WindowWidth = 2000;
		std::cout << "Window Level: " << m_WindowLevel << " Window Width: " << m_WindowWidth << std::endl;
	}
}

void SlicerBase::computeSlicesToSend(QVector2D mousePosition)
{
	// mousePosition is the local pixel location of the mouse click to the Slicer Widget -> Top left is (0,0)
	// Screen space starts upper left corner	

	// Model offset of (0,0,0) is in the center of the screen space (and texture)
	glm::vec3 sliceOffset = m_Camera.GetModelOffset();

	// For mapping sliceSelectLocation about (0,0)
	// ie: "treating" the middle of the screen as the origin for zoom
	QVector2D screenCenter = QVector2D(static_cast<float>(m_SlicerWidth) * (0.5f), static_cast<float>(m_SlicerHeight) * (0.5f));

	// Slice offset as screen pixels instead of screen clip space x[-1,1] y[-1,1]
	QVector2D sliceOffsetPixels = QVector2D(static_cast<float>(m_SlicerWidth) * 0.5f * sliceOffset.z, static_cast<float>(m_SlicerHeight) * 0.5f * sliceOffset.y);

	// Account for screen size being different than voxel dimensions (different aspect ratios)
	// ie: the voxel dimension a pixel should represent in screen's x and y direction
	float xFract = static_cast<float>(m_XVoxels) / static_cast<float>(m_SlicerWidth);
	float yFract = static_cast<float>(m_YVoxels) / static_cast<float>(m_SlicerHeight);

	float xLoc = mousePosition.x() - screenCenter.x();
	xLoc *= m_ZoomAmount;
	xLoc = xLoc - sliceOffsetPixels.x() + screenCenter.x();

	float yLoc = mousePosition.y() - screenCenter.y();
	//yLoc *= m_Zoom * m_CTData->getAxialAspectRatio();
	yLoc *= m_ZoomAmount;
	//yLoc = yLoc - sliceOffsetPixels.y() * m_CTData->getAxialAspectRatio() + screenCenter.y();
	yLoc = yLoc - sliceOffsetPixels.y() + screenCenter.y();

	// Map selected pixel location to voxel location
	xLoc *= xFract;
	yLoc *= yFract;

	// Send slice location to other slicers
	// Ensure float represents a "rounded" integer
	m_SlicesToSend = QVector2D(round(xLoc), round(yLoc));
	//emit baseSendSlice(QVector2D(round(xLoc), round(yLoc)));
}

QVector2D SlicerBase::getSlicesToSend()
{
	return m_SlicesToSend;
}

void SlicerBase::updateColorMap(ColormapSelect select)
{
	// Free old colormap
	mmOpenGLClearErrorMacro();
	glDeleteTextures(1, &m_ColormapTexture);
	mmOpenGLCheckErrorMacro("Error with changing color map");

	createColorMap(select);
}

void SlicerBase::updateShaderProgram(const char* vertexPath, const char* fragmentPath)
{
	// Free old shader program
	mmOpenGLClearErrorMacro();
	glDeleteProgram(m_ShaderProgram);
	mmOpenGLCheckErrorMacro("Error with updating vertex and fragment shader program");

	createShaderProgram(vertexPath, fragmentPath);
}

void SlicerBase::updateComputeShaderProgram(const char* computePath)
{
	// Free old compute program
	mmOpenGLClearErrorMacro();
	glDeleteProgram(m_ComputeProgram);
	mmOpenGLCheckErrorMacro("Error with updating compute shader program");

	createComputeShaderProgram(computePath);
}

void SlicerBase::createColorMap(ColormapSelect select)
{
	switch (select)
	{
	case GRAYSCALE:
		createGrayColorMap(NEAREST);
		break;
	default:
		createGrayColorMap(NEAREST);
		break;
	}
}

void SlicerBase::createSliceTexture()
{
	int numChannels = 1;
	int totalSize = m_XVoxels * m_YVoxels * numChannels;

	// Set all values to 255 and use only 1 channel (RED)
	std::vector<unsigned char> blankTexture(totalSize, 255);

	mmOpenGLClearErrorMacro();
	glGenTextures(1, &m_SliceTexture);
	glBindTexture(GL_TEXTURE_2D, m_SliceTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, m_XVoxels, m_YVoxels);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_XVoxels, m_YVoxels, GL_RED, GL_UNSIGNED_BYTE, &blankTexture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mmOpenGLCheckErrorMacro("Error with initializing blank texture - OpenGL");
}

void SlicerBase::createGrayColorMap(InterpolationSelect select)
{
	float grayLevels;
	int numColorChannels;
	int totalSize;
	int colorIdx;

	std::vector<unsigned char> grayColorMap;
	unsigned char* pColorMap;

	switch (select)
	{
	case LINEAR:

		grayLevels = 2;
		numColorChannels = 4;
		totalSize = grayLevels * numColorChannels;

		grayColorMap.resize(totalSize);
		pColorMap = &grayColorMap[0];

		colorIdx = 0;
		for (int i = 0; i < grayColorMap.size(); i = i + numColorChannels)
		{
			for (int j = 0; j < numColorChannels; j++)
			{
				if (j == 0)			// RED
					pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
				else if (j == 1)	// GREEN
					pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
				else if (j == 2)	// BLUE
					pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
				else				// ALPHA
					pColorMap[i + j] = 255;
			}
			colorIdx++;
		}

		mmOpenGLClearErrorMacro();
		glGenTextures(1, &m_ColormapTexture);
		glBindTexture(GL_TEXTURE_1D, m_ColormapTexture);
		glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGBA8, grayLevels);
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, grayLevels, GL_RGBA, GL_UNSIGNED_BYTE, &grayColorMap[0]);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		mmOpenGLCheckErrorMacro("Error with initializing gray colormap texture - OpenGL");
		break;

	case NEAREST:

		grayLevels = 256;
		numColorChannels = 4;
		totalSize = grayLevels * numColorChannels;

		grayColorMap.resize(totalSize);
		pColorMap = &grayColorMap[0];

		colorIdx = 0;
		for (int i = 0; i < totalSize; i = i + numColorChannels)
		{
			for (int j = 0; j < numColorChannels; j++)
			{
				if (j == 0)			// RED
					pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
				else if (j == 1)	// GREEN
					pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
				else if (j == 2)	// BLUE
					pColorMap[i + j] = static_cast<unsigned char>(255 * (colorIdx / (grayLevels - 1)));
				else				// ALPHA
					pColorMap[i + j] = 255;
			}
			colorIdx++;
		}

		mmOpenGLClearErrorMacro();
		glGenTextures(1, &m_ColormapTexture);
		glBindTexture(GL_TEXTURE_1D, m_ColormapTexture);
		glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGBA8, grayLevels);
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, grayLevels, GL_RGBA, GL_UNSIGNED_BYTE, &grayColorMap[0]);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		mmOpenGLCheckErrorMacro("Error with initializing gray colormap texture - OpenGL");
		break;

	default:

		std::cout << "Select interpolate or linear for creating gray color map" << std::endl;
		break;
	}
}

void SlicerBase::createShaderProgram(const char* vertexSource, const char* fragmentSource)
{
	mmOpenGLClearErrorMacro();
	
	GLuint vertex, fragment;
	vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
	checkCompileErrors(vertex, "VERTEX");
	fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
	checkCompileErrors(fragment, "FRAGMENT");

	m_ShaderProgram = glCreateProgram();
	glAttachShader(m_ShaderProgram, vertex);
	glAttachShader(m_ShaderProgram, fragment);
	glLinkProgram(m_ShaderProgram);
	checkCompileErrors(m_ShaderProgram, "PROGRAM");
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	mmOpenGLCheckErrorMacro("Error with compiling vertex and fragment program.");
}

void SlicerBase::createComputeShaderProgram(const char* computeSource)
{
	mmOpenGLClearErrorMacro();

	GLuint compute;
	compute = compileShader(GL_COMPUTE_SHADER, computeSource);
	checkCompileErrors(compute, "COMPUTE");

	m_ComputeProgram = glCreateProgram();
	glAttachShader(m_ComputeProgram, compute);
	glLinkProgram(m_ComputeProgram);
	checkCompileErrors(m_ComputeProgram, "PROGRAM");
	glDeleteShader(compute);

	mmOpenGLCheckErrorMacro("Error with compiling compute program.");
}

void SlicerBase::drawTextureQuad(int vertexCount /*= 6*/)
{
	mmOpenGLClearErrorMacro();

	// Binding the Vertex Array for drawing airway tree
	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);

	mmOpenGLCheckErrorMacro("OpenGL---Error with drawing texture geometry: SlicerBase");
}

void SlicerBase::createProjectionMatrix()
{
	m_Projection = glm::ortho(m_OrthoProjPos[0], m_OrthoProjPos[1], m_OrthoProjPos[2], m_OrthoProjPos[3]);
}

GLuint SlicerBase::compileShader(GLuint type, const char* shaderPath)
{
	const char* shaderType;
	if (type == GL_VERTEX_SHADER)
		shaderType = "Vertex";
	else if (type == GL_FRAGMENT_SHADER)
		shaderType = "Fragment";
	else if (type == GL_COMPUTE_SHADER)
		shaderType = "Compute";
	else
		shaderType = "Unknown";

	std::string code;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		// read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		// close file handlers
		shaderFile.close();
		// convert stream into string
		code = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ::" << shaderType << std::endl;
	}
	const char* shaderCode = code.c_str();
	unsigned int id = glCreateShader(type);
	glShaderSource(id, 1, &shaderCode, 0);
	glCompileShader(id);

	std::cout << "COMPILED SHADER::" << shaderType << std::endl;

	return id;
}

void SlicerBase::checkCompileErrors(GLuint shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type:" << type << "\n" << infoLog << "\n ------------------------------------------------------ " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n ------------------------------------------------------ " << std::endl;
		}
	}
}

void SlicerBase::createTextureQuad(TextureFlipSelect select)
{
	std::vector<GLfloat> vertices = {
		// positions			// texture coords (reversed)
		1.0f,  1.0f,  0.0f,    1.0f, 1.0f, // top right
		1.0f, -1.0f,  0.0f,    1.0f, 0.0f, // bottom right
		-1.0f, -1.0f,  0.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f,  0.0f,   0.0f, 1.0f  // top left 
	};

	std::vector<GLuint> indices = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	switch (select)
	{
	case NONE:

		mmOpenGLClearErrorMacro();

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

		// Position Attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));
		glEnableVertexAttribArray(0);
		// Texture Attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		mmOpenGLCheckErrorMacro("Error with initializing texture geometry - OpenGL");
		break;

	case VERTICALLY:

		// flip vertically
		for (int i = 0; i < vertices.size(); i++)
		{
			if ((i + 1) % 5 == 0)
			{
				vertices[i] = 1.0f - vertices[i];
			}
		}

		mmOpenGLClearErrorMacro();

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

		// Position Attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));
		glEnableVertexAttribArray(0);
		// Texture Attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		mmOpenGLCheckErrorMacro("Error with initializing texture geometry - OpenGL");
		break;
	}
}

void SlicerBase::createCTVolumeTexture()
{
	m_CTData->init3DTexture();
	m_CTTexture = m_CTData->getTextureID();
}

