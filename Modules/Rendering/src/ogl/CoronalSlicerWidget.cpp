#include "CoronalSlicerWidget.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "rendering.configure.h"

CoronalSlicerWidget::CoronalSlicerWidget(CTVolume *CTData, MainWindow *mw) :
	m_MainWindow(mw),
	m_CTData(CTData),
	m_CoronalSliceSelect(0),
	m_MouseRightClick(false),
	m_MouseLeftClick(false)
{
	setFocusPolicy(Qt::StrongFocus);
}

CoronalSlicerWidget::CoronalSlicerWidget(CTVolume *CTData, QWidget *parent) :
	m_CoronalSliceSelect(0),
	m_CTData(CTData),
	m_MouseRightClick(false),
	m_MouseLeftClick(false)
{
	setFocusPolicy(Qt::StrongFocus);
}

CoronalSlicerWidget::~CoronalSlicerWidget()
{
}

void CoronalSlicerWidget::axialChangeSlice(QVector2D sliceSelectLocation)
{
	m_CoronalSliceSelect = static_cast<int>(sliceSelectLocation.y());
	
	// Bound Checking
	if (m_CoronalSliceSelect < 0)
	{
		m_CoronalSliceSelect = 0;
	}
	else if (m_CoronalSliceSelect > m_CTData->getNumRows() - 1)
	{
		m_CoronalSliceSelect = m_CTData->getNumRows() - 1;
	}
	else
	{
		// do nothing
	}
	m_SlicerBase.updateSliceSelect(m_CoronalSliceSelect);
	m_SlicerBase.cursorOff();
	emit clearVoxelInfo();
	update();	
}

void CoronalSlicerWidget::sagittalChangeSlice(QVector2D sliceSelectLocation)
{
	m_CoronalSliceSelect = static_cast<int>(sliceSelectLocation.x());

	// Bound Checking
	if (m_CoronalSliceSelect < 0)
	{
		m_CoronalSliceSelect = 0;
	}
	else if (m_CoronalSliceSelect > m_CTData->getNumRows() - 1)
	{
		m_CoronalSliceSelect = m_CTData->getNumRows() - 1;
	}
	else
	{
		// do nothing
	}
	m_SlicerBase.updateSliceSelect(m_CoronalSliceSelect);
	m_SlicerBase.cursorOff();
	emit clearVoxelInfo();
	update();	
}

void CoronalSlicerWidget::receiveVoxelInfo(QVector4D voxelInfo)
{
	m_SlicerBase.cursorOff();
	emit clearVoxelInfo();
	update();
}

void CoronalSlicerWidget::initializeGL()
{
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (gladLoadGL())
		qDebug() << "GLAD Loaded";
	else
		qDebug() << "Error Loading GLAD";

	qDebug() << "OpenGL Version: " << glGetString(GL_VERSION);
	qDebug() << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
	
	glClearColor(0.5f, 0.1f, 0.1f, 1.0f);

	m_SlicerBase.initialize(m_CTData->getNumColumns(),m_CTData->getNumSlices(),width(),height(),m_CTData);
	
	m_SlicerBase.initializeTextures();
	m_SlicerBase.initializeShader(RENDERING_RESOURCES_DIR "/ogl/SlicerVertex.glsl", RENDERING_RESOURCES_DIR "/ogl/SlicerFragment.glsl");
	m_SlicerBase.initializeComputeShader(RENDERING_RESOURCES_DIR "/ogl/CoronalSliceCompute.glsl");
	m_SlicerBase.initializeCrosshair();
}

void CoronalSlicerWidget::resizeGL(int w, int h)
{
	m_SlicerBase.updateSlicerDimensions(w, h);
}

void CoronalSlicerWidget::paintGL()
{
	m_SlicerBase.draw(SlicerBase::CORONAL);
	
}

void CoronalSlicerWidget::sendSliceSelect(QVector2D sliceSelectLocation)
{
	// Send slice location to other slicers
	// Ensure float represents a "rounded" integer
	emit coronalSliceSelect(sliceSelectLocation);

	// To select voxel from CT Data
	int xVox = static_cast<int>(round(sliceSelectLocation.x()));
	int yVox = static_cast<int>(m_CoronalSliceSelect);
	int zVox = static_cast<int>(round(sliceSelectLocation.y()));
	int huVal = static_cast<int>(std::round(m_CTData->getVoxel(xVox, yVox, zVox)));

	// Send information to SlicerMain for display
	emit sendVoxelInfo(QVector4D(xVox, yVox, zVox, huVal));

	// Send information to SlicerMain for display
	//emit sendVoxelInfo(QVector4D(xVox, yVox, zVox, m_CTData->getVoxel(xVox, yVox, zVox)));
}

void CoronalSlicerWidget::keyPressEvent(QKeyEvent *e)
{
	m_SlicerBase.keyOnPress(e->key());
	
	switch (e->key())
	{
	case Qt::Key_Escape:
		exit(0);
		break;
	case Qt::Key_W:
		m_CoronalSliceSelect++;
		if (m_CoronalSliceSelect >= m_CTData->getNumRows())
			m_CoronalSliceSelect = m_CTData->getNumRows() - 1;
		m_SlicerBase.updateSliceSelect(m_CoronalSliceSelect);
		m_SlicerBase.cursorOff();
		emit clearVoxelInfo();
		break;
	case Qt::Key_S:
		m_CoronalSliceSelect--;
		if (m_CoronalSliceSelect < 0)
			m_CoronalSliceSelect = 0;
		m_SlicerBase.updateSliceSelect(m_CoronalSliceSelect);
		m_SlicerBase.cursorOff();
		emit clearVoxelInfo();
		break;
	default:
		break;
	}

	update();
	e->accept();  // Don't pass any key events to parent
}

void CoronalSlicerWidget::mousePressEvent(QMouseEvent *e)
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
		sendSliceSelect(m_SlicerBase.getSlicesToSend());
		m_SlicerBase.cursorOn();
		break;
	default:
		m_MouseLeftClick = false;
		m_MouseRightClick = false;
		break;
	}
	update();
}

void CoronalSlicerWidget::mouseMoveEvent(QMouseEvent *e)
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

void CoronalSlicerWidget::mouseReleaseEvent(QMouseEvent *e)
{
	m_SlicerBase.mouseOnRelease();
	m_MouseLeftClick = false;
	m_MouseRightClick = false;
	update();
}

void CoronalSlicerWidget::wheelEvent(QWheelEvent *e)
{
	m_SlicerBase.mouseOnWheel(e->angleDelta().y());
	emit clearVoxelInfo();
	update();
}

