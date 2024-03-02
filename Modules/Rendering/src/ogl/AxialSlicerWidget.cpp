#include "AxialSlicerWidget.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Logger.h"

#include "rendering.configure.h"

AxialSlicerWidget::AxialSlicerWidget(CTVolume *CTData, MainWindow *mw) :
	m_MainWindow(mw),
	m_CTData(CTData),
	m_AxialSliceSelect(0),
	m_MouseLeftClick(false),
	m_MouseRightClick(false)
{
	qDebug() << "Axial Slicer Widget ctor";
	setFocusPolicy(Qt::StrongFocus);	
}

AxialSlicerWidget::AxialSlicerWidget(CTVolume *CTData, QWidget *parent /*=0*/) :
	m_AxialSliceSelect(0),
	m_CTData(CTData),
	m_MouseLeftClick(false),
	m_MouseRightClick(false)
{
	setFocusPolicy(Qt::StrongFocus);
}

AxialSlicerWidget::~AxialSlicerWidget()
{
	// Make sure the context is current when deleting the texture
	// and the buffers.
	
	//delete texture;
	//makeCurrent();
	//glDeleteTextures(1, &m_SliceTexture);
	//glDeleteTextures(1, &m_ColormapTexture);
	//glDeleteProgram(m_SlicerShaderProgram);
	//glDeleteProgram(m_SlicerComputeProgram);
	//glDeleteVertexArrays(1, &m_VertexArray);
	//glDeleteBuffers(1, &m_VertexBuffer);
	//glDeleteBuffers(1, &m_IndexBuffer);
	//doneCurrent();
	//this->makeCurrent();
	//m_SlicerBase.releaseResources();
	//this->doneCurrent();
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
	m_SlicerBase.updateSliceSelect(m_AxialSliceSelect);
	m_SlicerBase.cursorOff();
	emit clearVoxelInfo();
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
	m_SlicerBase.updateSliceSelect(m_AxialSliceSelect);
	m_SlicerBase.cursorOff();
	emit clearVoxelInfo();
	update();	
}

void AxialSlicerWidget::receiveVoxelInfo(QVector4D voxelInfo)
{
	m_SlicerBase.cursorOff();
	emit clearVoxelInfo();
	update();
}

void AxialSlicerWidget::initializeGL()
{
	if (gladLoadGL())
		qDebug() << "GLAD Loaded";
	else
		qDebug() << "Error Loading GLAD";

	qDebug() << "OpenGL Version: " << glGetString(GL_VERSION);
	qDebug() << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);

	glClearColor(0.5f, 0.1f, 0.1f, 1.0f);	
	
	m_SlicerBase.initialize(m_CTData->getNumColumns(), m_CTData->getNumRows(), width(), height(), m_CTData);
	
	m_SlicerBase.initializeTextures();
	m_SlicerBase.initializeShader(RENDERING_RESOURCES_DIR "/ogl/SlicerVertex.glsl", RENDERING_RESOURCES_DIR "/ogl/SlicerFragment.glsl");
	m_SlicerBase.initializeComputeShader(RENDERING_RESOURCES_DIR "/ogl/AxialSliceCompute.glsl");
	m_SlicerBase.initializeCrosshair();

	//m_Crosshair.init(glm::vec2(0.0f, 0.0f));
}

void AxialSlicerWidget::resizeGL(int w, int h)
{
	m_SlicerBase.updateSlicerDimensions(w, h);
	//m_ScreenWidth = w;
	//m_ScreenHeight = h;
}

void AxialSlicerWidget::paintGL()
{
	m_SlicerBase.draw(SlicerBase::AXIAL);
}

void AxialSlicerWidget::sendSliceSelect(QVector2D sliceSelectLocation)
{
	// Send slice location to other slicers
	// Ensure float represents a "rounded" integer
	emit axialSliceSelect(sliceSelectLocation);

	// To select voxel from CT Data
	int xVox = static_cast<int>(round(sliceSelectLocation.x()));
	int yVox = static_cast<int>(round(sliceSelectLocation.y()));
	int zVox = static_cast<int>(m_AxialSliceSelect);
	int huVal = static_cast<int>(std::round(m_CTData->getVoxel(xVox, yVox, zVox)));

	// Send information to SlicerMain for display
	emit sendVoxelInfo(QVector4D(xVox, yVox, zVox, huVal));

	// Send information to SlicerMain for display
	//emit sendVoxelInfo(QVector4D(xVox, yVox, zVox, m_CTData->getVoxel(xVox, yVox, zVox)));
}

void AxialSlicerWidget::mousePressEvent(QMouseEvent *e)
{
	m_SlicerBase.mouseOnClick(QVector2D(e->pos()), e->button());

	// TODO: Add support so that SlicerBase sends the Signal rather than getting from Base and sending through sendSliceSelect
	// TODO: Add support so that SlicerBase see's OpenGL resources when it contains the Crosshair - Otherwise SlicerBase's vertex array and vertex buffer are the same ID as the crosshair vertex array and buffer
	switch (e->button())
	{
	case Qt::LeftButton:
		m_MouseLeftClick = true;
		break;
	case Qt::RightButton:
		m_MouseRightClick = true;
		//m_Crosshair.updatePosition(QVector2D(e->pos()), width(), height());
		m_SlicerBase.cursorOn();
		sendSliceSelect(m_SlicerBase.getSlicesToSend());
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
	m_SlicerBase.mouseOnMove(QVector2D(e->pos()));

	// TODO: Add support so that SlicerBase sends the Signal rather than getting from Base and sending through sendSliceSelect
	// Using mouse flags allows the other slices to be updated in real time.
	if (m_MouseLeftClick)
	{
		m_SlicerBase.cursorOff();
		emit clearVoxelInfo();
	}
	else if (m_MouseRightClick)
	{
		//m_Crosshair.updatePosition(QVector2D(e->pos()), width(), height());
		sendSliceSelect(m_SlicerBase.getSlicesToSend());
	}
	else
	{
		// do nothing
	}
	update();
}

void AxialSlicerWidget::mouseReleaseEvent(QMouseEvent *e)
{
	m_SlicerBase.mouseOnRelease();
	m_MouseLeftClick = false;
	m_MouseRightClick = false;
	update();
}

void AxialSlicerWidget::wheelEvent(QWheelEvent *e)
{
	m_SlicerBase.mouseOnWheel(e->angleDelta().y());
	emit clearVoxelInfo();
	update();	
}

void AxialSlicerWidget::keyPressEvent(QKeyEvent *e)
{
	m_SlicerBase.keyOnPress(e->key());
	
	switch (e->key())
	{
	case Qt::Key_Escape:
		exit(0);
		break;
	case Qt::Key_W:
		m_AxialSliceSelect++;
		if (m_AxialSliceSelect >= m_CTData->getNumSlices())
			m_AxialSliceSelect = m_CTData->getNumSlices() - 1;
		m_SlicerBase.updateSliceSelect(m_AxialSliceSelect);
		m_SlicerBase.cursorOff();
		emit clearVoxelInfo();
		break;
	case Qt::Key_S:
		m_AxialSliceSelect--;
		if (m_AxialSliceSelect < 0)
			m_AxialSliceSelect = 0;
		m_SlicerBase.updateSliceSelect(m_AxialSliceSelect);
		m_SlicerBase.cursorOff();
		emit clearVoxelInfo();
		break;
	default:
		break;
	}

	update();
	e->accept();  // Don't pass any key events to parent
}


