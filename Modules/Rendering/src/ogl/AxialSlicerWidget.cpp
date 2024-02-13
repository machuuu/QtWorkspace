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
	m_MouseLeftClick(false),
	m_MouseRightClick(false)
{
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
	update();
}

void AxialSlicerWidget::initializeGL()
{
	if (gladLoadGL())
		std::cout << "GLAD Loaded" << std::endl;
	else
		std::cout << "Error Loading GLAD" << std::endl;

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glClearColor(0.5f, 0.1f, 0.1f, 1.0f);	
	
	m_SlicerBase = SlicerBase(m_CTData->getNumColumns(), m_CTData->getNumRows(), width(), height(), m_CTData);
	
	m_SlicerBase.initializeTextures();
	m_SlicerBase.initializeShader(SHADER_DIR "/SlicerVertex.glsl", SHADER_DIR "/SlicerFragment.glsl");
	m_SlicerBase.initializeComputeShader(SHADER_DIR "/AxialSliceCompute.glsl");
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
	//int xVox = static_cast<int>(xLoc);
	//int yVox = static_cast<int>(yLoc);
	//int zVox = static_cast<int>(m_AxialSliceSelect);

	int t1 = 1;
	int t2 = 1;
	int t3 = 1;
	int t4 = 1;

	// Send information to SlicerMain for display
	emit sendVoxelInfo(QVector4D(t1, t2, t3, t4));
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
	m_SlicerBase.mouseOnWheel(e->delta());
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
		break;
	case Qt::Key_S:
		m_AxialSliceSelect--;
		if (m_AxialSliceSelect < 0)
			m_AxialSliceSelect = 0;
		m_SlicerBase.updateSliceSelect(m_AxialSliceSelect);
		break;
	default:
		break;
	}

	update();
	e->accept();  // Don't pass any key events to parent
}


