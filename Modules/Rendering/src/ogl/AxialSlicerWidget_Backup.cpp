#include "AxialSlicerWidget.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

AxialSlicerWidget::AxialSlicerWidget(CTVolume *CTData, MainWindow *mw) :
	m_MainWindow(mw),
	m_CTData(CTData),
	m_AxialSliceSelect(0),
	m_Zoom(1.0f),
	m_MouseRightClick(false),
	m_MouseLeftClick(false)
{
	setFocusPolicy(Qt::StrongFocus);
	
}

AxialSlicerWidget::AxialSlicerWidget(CTVolume *CTData, QWidget *parent /*=0*/) :
	m_AxialSliceSelect(0),
	m_CTData(CTData),
	m_Zoom(1.0f),
	m_MouseRightClick(false),
	m_MouseLeftClick(false)
{
	setFocusPolicy(Qt::StrongFocus);
}

AxialSlicerWidget::~AxialSlicerWidget()
{
	// Make sure the context is current when deleting the texture
	// and the buffers.
	
	//delete texture;
	makeCurrent();
	glDeleteTextures(1, &m_SliceTexture);
	glDeleteTextures(1, &m_ColormapTexture);
	glDeleteProgram(m_SlicerShaderProgram);
	glDeleteProgram(m_SlicerComputeProgram);
	glDeleteVertexArrays(1, &m_VertexArray);
	glDeleteBuffers(1, &m_VertexBuffer);
	glDeleteBuffers(1, &m_IndexBuffer);
	doneCurrent();
}

void AxialSlicerWidget::sagittalChangeSlice(QVector2D sliceSelectLocation)
{
	m_AxialSliceSelect = static_cast<int>(sliceSelectLocation.y());

	// Bound Checking
	if (m_AxialSliceSelect < 0)
	{
		m_AxialSliceSelect = 0;
	}
	else if (m_AxialSliceSelect > m_CTData->getNumSlices() - 1)
	{
		m_AxialSliceSelect = m_CTData->getNumSlices() - 1;
	}
	else
	{
		// do nothing
	}
	update();
}

void AxialSlicerWidget::coronalChangeSlice(QVector2D sliceSelectLocation)
{
	m_AxialSliceSelect = static_cast<int>(sliceSelectLocation.y());

	// Bound Checking
	if (m_AxialSliceSelect < 0)
	{
		m_AxialSliceSelect = 0;
	}
	else if (m_AxialSliceSelect > m_CTData->getNumSlices() - 1)
	{
		m_AxialSliceSelect = m_CTData->getNumSlices() - 1;
	}
	else
	{
		// do nothing
	}
	update();
}

void AxialSlicerWidget::initializeGL()
{
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (gladLoadGL())
		std::cout << "GLAD Loaded" << std::endl;
	else
		std::cout << "Error Loading GLAD" << std::endl;

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glClearColor(0.5f, 0.1f, 0.1f, 1.0f);

	// Set speed for panning the Texture
	m_Camera.SetMouseMovementSpeed(0.0025f);
	// Set sensitivity for zooming
	m_ZoomSensitivity = 0.01f;
	// Texture Bounds define the shape of the surface to draw the texture on
	m_Scale = glm::mat4(1.0f);
	m_Scale[0][0] = 1.0f;										// CTVolume's X as screen X
	m_Scale[1][1] = 1.0f / m_CTData->getAxialAspectRatio();		// CTVolume's Y as screen Y
	m_Scale[2][2] = 1.0f;										// CTVolume's Z as slice control
	m_Scale[3][3] = 1.0f;										// Homogeneous coordinate
	//m_TextureBounds = glm::vec4(-1.0f, 1.0f, -1.0f / m_CTData->getAxialAspectRatio(), 1.0f / m_CTData->getAxialAspectRatio()); //Left|Right|Bottom|Top
	m_OrthoProjPos = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	// Initialize Model, View and Projection Matrices
	m_Model = glm::mat4(1.0f);
	m_View = glm::mat4(1.0f);
	
	constructProjectionMatrix();

	initTextureShaders();
	initTexture();
	initTextureGeometry();

	m_Crosshair.init(glm::vec2(0.0f, 0.0f));
}

void AxialSlicerWidget::resizeGL(int w, int h)
{
	m_ScreenWidth = w;
	m_ScreenHeight = h;
}

void AxialSlicerWidget::paintGL()
{
	//makeCurrent();

	/* Uniform declarations */
	//GLuint sliceSelect = m_SliceSelect;
	std::cout << "Slice: " << m_AxialSliceSelect << std::endl;

	float windowLevel, windowWidth;
	SlicerHelper::getWindowParameters(windowLevel, windowWidth, SlicerHelper::MEDIASTINUM);

	mmOpenGLClearErrorMacro();

	/* COMPUTATION */
	// TODO: Understand WORKGROUPS BETTER
	//QSize workGroups = getWorkGroups(32);
	//GLuint volumeID = m_CTData->init3DTexture();
	// Bind CT Texture as an image layer unit for reading into compute shader
	//glBindImageTexture(0, m_CTTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
	m_CTData->bind3DImageTexture(0);
	// Bind Slice Texture as an image layer unit for writing from compute shader
	glBindImageTexture(1, m_SliceTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	// Bind Compute Shader: Creates 2D texture (slice) from 3D texture volume
	glUseProgram(m_SlicerComputeProgram);
	glUniform1i(glGetUniformLocation(m_SlicerComputeProgram, "sliceSelect"), m_AxialSliceSelect);
	//glDispatchCompute(m_CTData.getNumColumns(), m_CTData.getNumRows(), m_CTData.getNumSlices());
	glDispatchCompute(m_CTData->getNumColumns(), m_CTData->getNumRows(), 1);
	//glDispatchCompute(workGroups.width(), workGroups.height(), 1);
	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	// Compute shader clean up
	//glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
	m_CTData->unbind3DImageTexture();
	glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

	/* DISPLAY */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Bind Textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_SliceTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, m_ColormapTexture);
	glUseProgram(m_SlicerShaderProgram);
	// Bind Vertex and Fragment Shader: 
	m_Scale = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(m_SlicerShaderProgram, "model"), 1, GL_FALSE, &m_Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_SlicerShaderProgram, "scale"), 1, GL_FALSE, &m_Scale[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_SlicerShaderProgram, "view"), 1, GL_FALSE, &m_View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_SlicerShaderProgram, "projection"), 1, GL_FALSE, &m_Projection[0][0]);
	glUniform1f(glGetUniformLocation(m_SlicerShaderProgram, "windowLevel"), windowLevel);
	glUniform1f(glGetUniformLocation(m_SlicerShaderProgram, "windowWidth"), windowWidth);
	mmOpenGLCheckErrorMacro("Error with computing and painting texture - OPENGL");

	drawTextureGeometry();
	m_Crosshair.draw();

	glUseProgram(0);
}

void AxialSlicerWidget::initTexture()
{
	// TODO: Figure out how to keep CTFlatData as short instead of float. When setting internal format to GL_R32F from GL_RED and GL_SHORT, imageLoad in fragment shader only reads normalized values [0-1.0f]
	m_CTData->init3DTexture();

	m_SliceTexture = SlicerHelper::getBlankTexture(m_CTData->getNumColumns(), m_CTData->getNumRows(), 1);
	
	m_ColormapTexture = SlicerHelper::getGrayColorMap(SlicerHelper::LINEAR);
}

void AxialSlicerWidget::initTextureGeometry()
{
	SlicerHelper::getTextureQuad(m_VertexArray, m_VertexBuffer, m_IndexBuffer, SlicerHelper::VERTICALLY);
}

void AxialSlicerWidget::initTextureShaders()
{
	m_SlicerShaderProgram = SlicerHelper::getShaderProgram(DATA_DIR "/SlicerVertex.glsl", DATA_DIR "/SlicerFragment.glsl");
	m_SlicerComputeProgram = SlicerHelper::getComputeShaderProgram(DATA_DIR "/AxialSliceCompute.glsl");
}

void AxialSlicerWidget::sendSliceSelect(QVector2D sliceSelectLocation)
{
	// sliceSelectLocation is the local pixel location of the mouse click to the Slicer Widget -> Top left is (0,0)
	// Screen space starts upper left corner	

	// Model offset of (0,0,0) is in the center of the screen space (and texture)
	glm::vec3 sliceOffset = m_Camera.GetModelOffset();

	// For mapping sliceSelectLocation about (0,0)
	// ie: "treating" the middle of the screen as the origin for zoom
	QVector2D screenCenter = QVector2D(static_cast<float>(m_ScreenWidth) * (0.5f), static_cast<float>(m_ScreenHeight) * (0.5f));

	// Slice offset as screen pixels instead of screen clip space x[-1,1] y[-1,1]
	QVector2D sliceOffsetPixels = QVector2D(static_cast<float>(m_ScreenWidth) * 0.5f * sliceOffset.x, static_cast<float>(m_ScreenHeight) * 0.5f * sliceOffset.y);

	// Account for screen size being different than voxel dimensions (different aspect ratios)
	// ie: the voxel dimension a pixel should represent in screen's x and y direction
	float xFract = static_cast<float>(m_CTData->getNumColumns()) / static_cast<float>(m_ScreenWidth);
	float yFract = static_cast<float>(m_CTData->getNumRows()) / static_cast<float>(m_ScreenHeight);

	float xLoc = sliceSelectLocation.x() - screenCenter.x();
	xLoc *= m_Zoom;
	xLoc = xLoc - sliceOffsetPixels.x() + screenCenter.x();

	float yLoc = sliceSelectLocation.y() - screenCenter.y();
	//yLoc *= m_Zoom * m_CTData->getAxialAspectRatio();
	yLoc *= m_Zoom;
	//yLoc = yLoc - sliceOffsetPixels.y() * m_CTData->getAxialAspectRatio() + screenCenter.y();
	yLoc = yLoc - sliceOffsetPixels.y() + screenCenter.y();

	// Map selected pixel location to voxel location
	xLoc *= xFract;
	yLoc *= yFract;

	// Send slice location to other slicers
	// Ensure float represents a "rounded" integer
	emit axialSliceSelect(QVector2D(round(xLoc), round(yLoc)));

	// To select voxel from CT Data
	int xVox = static_cast<int>(xLoc);
	int yVox = static_cast<int>(yLoc);
	int zVox = static_cast<int>(m_AxialSliceSelect);

	// Send information to SlicerMain for display
	emit sendVoxelInfo(QVector4D(round(xLoc), round(yLoc), round(m_AxialSliceSelect), m_CTData->getVoxel(xVox, yVox, zVox)));
}

void AxialSlicerWidget::getWindowParameters(float &windowLevel, float &windowWidth, WindowType select)
{
	switch (select) {
	case LUNG:
		windowLevel = -350;
		windowWidth = 1800;
		break;
	case MEDIASTINUM:
		windowLevel = -40;
		windowWidth = 400;
		break;
	case XSECTION:
		windowLevel = -900;
		windowWidth = 600;
		break;
	default:
		windowLevel = 0;
		windowWidth = 2000;
		std::cout << "Window Level: " << windowLevel << " Window Width: " << windowWidth << std::endl;
	}
}


void AxialSlicerWidget::drawTextureGeometry()
{
	SlicerHelper::drawTextureQuad(m_VertexArray, m_VertexBuffer, m_IndexBuffer);
}

void AxialSlicerWidget::mousePressEvent(QMouseEvent *e)
{
	m_MouseLastPosition = QVector2D(e->pos());

	switch (e->button())
	{
	case Qt::LeftButton:
		m_MouseLeftClick = true;
		break;
	case Qt::RightButton:
		m_MouseRightClick = true;
		m_Crosshair.updatePosition(QVector2D(e->pos()),m_ScreenWidth,m_ScreenHeight);
		sendSliceSelect(QVector2D(e->pos()));
		break;
	default:
		m_MouseLeftClick = false;
		m_MouseRightClick = false;
		break;
	}
	update();
}

void AxialSlicerWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (m_MouseLeftClick)
	{
		//glm::vec3 modelOffset = m_Camera.GetModelOffset();

		////Do nothing if Orthogonal Projection Window goes outside of the Texture Bounds
		//if (m_OrthoProjPos[0] < m_TextureBounds[0])
		//{
		//	m_OrthoProjPos[0] = -1.0f;
		//	m_OrthoProjPos[1] = -1.0f + 2.0f * m_Zoom;
		//	m_OrthoProjPos[2] = -1.0f * m_Zoom + modelOffset.y;
		//	m_OrthoProjPos[3] =  1.0f * m_Zoom + modelOffset.y;

		//	constructProjectionMatrix();
		//}
		//else if (m_OrthoProjPos[1] > m_TextureBounds[1])
		//{
		//	m_OrthoProjPos[0] =  1.0f - 2.0f * m_Zoom;
		//	m_OrthoProjPos[1] =  1.0f;
		//	m_OrthoProjPos[2] = -1.0f * m_Zoom + modelOffset.y;
		//	m_OrthoProjPos[3] =  1.0f * m_Zoom + modelOffset.y;

		//	constructProjectionMatrix();
		//}
		//else if (m_OrthoProjPos[2] < m_TextureBounds[2])
		//{
		//	m_OrthoProjPos[0] = -1.0f * m_Zoom - modelOffset.x;
		//	m_OrthoProjPos[1] =  1.0f * m_Zoom - modelOffset.x;
		//	m_OrthoProjPos[2] = -1.0f;
		//	m_OrthoProjPos[3] = -1.0f + 2.0f * m_Zoom;

		//	constructProjectionMatrix();
		//}
		//else if (m_OrthoProjPos[3] > m_TextureBounds[3])
		//{
		//	m_OrthoProjPos[0] = -1.0f * m_Zoom - modelOffset.x;
		//	m_OrthoProjPos[1] =  1.0f * m_Zoom - modelOffset.x;
		//	m_OrthoProjPos[2] =  1.0f - 2.0f * m_Zoom;
		//	m_OrthoProjPos[3] =  1.0f;

		//	constructProjectionMatrix();
		//}
		//else // Calculate new position and update orthogonal projection window
		{

		QVector2D mouseLoc = QVector2D(e->pos());

		float xoffset = mouseLoc.x() - m_MouseLastPosition.x();
		float yoffset = mouseLoc.y() - m_MouseLastPosition.y();

		m_Camera.ProcessMouseTranslation(xoffset, yoffset, false);

		m_MouseLastPosition = QVector2D(e->pos());

		updateProjectionMatrix(fromPan);
		}		
	}
	else if (m_MouseRightClick)
	{
		sendSliceSelect(QVector2D(e->pos()));
		m_Crosshair.updatePosition(QVector2D(e->pos()), m_ScreenWidth, m_ScreenHeight);
		// Do nothing if not left click
	}
	else
	{
		// do nothing
	}
	update();
}

void AxialSlicerWidget::mouseReleaseEvent(QMouseEvent *e)
{
	m_MouseLeftClick = false;
	m_MouseRightClick = false;
}

void AxialSlicerWidget::wheelEvent(QWheelEvent *e)
{
	// Scroll up zoom inwards, Scroll down zoom outwards
	// Divide by 120 to get each scroll click to be 1.0f and adjust using 0.01f sensitivity
	m_Zoom += (e->delta() / 120.0f) * m_ZoomSensitivity;

	if (m_Zoom > 1.0f)
		m_Zoom = 1.0f;
	
	if (m_Zoom < m_ZoomSensitivity)	// Avoid 0.0f -> Set to smallest unit of zoom
		m_Zoom = m_ZoomSensitivity;

	// TODO: (SIMPLE) As you zoom in, reduce the movement speed
	m_Camera.SetMouseMovementSpeed(m_ZoomSensitivity * m_Zoom);

	updateProjectionMatrix(fromZoom);

	update();
}

void AxialSlicerWidget::updateProjectionMatrix(UpdateType typeSelect)
{
	
	float xCenter;
	float yCenter;
	
	switch (typeSelect)
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
		m_OrthoProjPos[0] = -m_Zoom;
		m_OrthoProjPos[1] =  m_Zoom;
		m_OrthoProjPos[2] = -m_Zoom;
		m_OrthoProjPos[3] =  m_Zoom;
		// Move back to location
		m_OrthoProjPos[0] += xCenter;
		m_OrthoProjPos[1] += xCenter;
		m_OrthoProjPos[2] += yCenter;
		m_OrthoProjPos[3] += yCenter;

		break;

	case fromPan:

		m_OrthoProjPos[0] = -1.0f * m_Zoom - m_Camera.GetModelOffset().x;
		m_OrthoProjPos[1] = 1.0f * m_Zoom - m_Camera.GetModelOffset().x;
		m_OrthoProjPos[2] = -1.0f * m_Zoom + m_Camera.GetModelOffset().y;
		m_OrthoProjPos[3] = 1.0f * m_Zoom + m_Camera.GetModelOffset().y;

		break;

	default:
		break;
	}
		
	constructProjectionMatrix();

}

void AxialSlicerWidget::constructProjectionMatrix()
{
	m_Projection = glm::ortho(m_OrthoProjPos[0], m_OrthoProjPos[1], m_OrthoProjPos[2], m_OrthoProjPos[3]);
}

void AxialSlicerWidget::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Escape:
		exit(0);
		break;
	case Qt::Key_W:
		m_AxialSliceSelect++;
		if (m_AxialSliceSelect >= m_CTData->getNumSlices())
			m_AxialSliceSelect = m_CTData->getNumSlices() - 1;
		update();
		break;
	case Qt::Key_S:
		m_AxialSliceSelect--;
		if (m_AxialSliceSelect < 0)
			m_AxialSliceSelect = 0;
		update();
		break;
	case Qt::Key_T:
		m_Zoom = 1.0f;
		m_Camera.SetModelOffset(glm::vec3(0.0f, 0.0f, 0.0f));
		m_Camera.SetMouseMovementSpeed(m_ZoomSensitivity * m_Zoom);
		m_OrthoProjPos = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
		constructProjectionMatrix();
		update();
		break;
	default:
		break;
	}

	e->accept();  // Don't pass any key events to parent
}


