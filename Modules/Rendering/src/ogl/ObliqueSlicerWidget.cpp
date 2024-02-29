#include "ObliqueSlicerWidget.h"
#include "rendering.configure.h"

ObliqueSlicerWidget::ObliqueSlicerWidget(CTVolume *CTData, MainWindow *mw) :
	m_MainWindow(mw),
	m_CTData(CTData)
{

}

ObliqueSlicerWidget::ObliqueSlicerWidget(CTVolume *CTData, QWidget *parent /*= 0*/) :
	m_CTData(CTData),
	m_MouseLeftClick(false),
	m_MouseRightClick(false)
{

}

ObliqueSlicerWidget::~ObliqueSlicerWidget()
{

}

void ObliqueSlicerWidget::receiveVoxelInfo(QVector4D voxelInfo)
{
	m_ObliqueSlicerBase.setOrbitCenter(voxelInfo.x(), voxelInfo.y(), voxelInfo.z());
	m_ObliqueSlicerBase.cursorOff();
	emit clearVoxelInfo();
	this->update();
}

void ObliqueSlicerWidget::initializeGL()
{
	if (gladLoadGL())
		std::cout << "GLAD Loaded" << std::endl;
	else
		std::cout << "Error Loading GLAD" << std::endl;

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	m_ObliqueSlicerBase.initialize(m_CTData->getNumColumns(), m_CTData->getNumRows(), m_CTData->getNumSlices(), width(), height(), m_CTData);
	
	GLint maxWorkGroupInvo;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxWorkGroupInvo);
	std::cout << "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS " << maxWorkGroupInvo << std::endl;

	GLint maxComputeWorkGroupCount[3] = { 0 };
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxComputeWorkGroupCount[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxComputeWorkGroupCount[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxComputeWorkGroupCount[2]);
	std::cout << "GL_MAX_COMPUTE_WORK_GROUP_COUNT " << maxComputeWorkGroupCount[0] << " " << maxComputeWorkGroupCount[1] << " " << maxComputeWorkGroupCount[2] << std::endl;

	GLint maxComputeWorkGroupSize[3] = { 0 };
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxComputeWorkGroupSize[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxComputeWorkGroupSize[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxComputeWorkGroupSize[2]);
	std::cout << "GL_MAX_COMPUTE_WORK_GROUP_SIZE " << maxComputeWorkGroupSize[0] << " " << maxComputeWorkGroupSize[1] << " " << maxComputeWorkGroupSize[2] << std::endl;


	m_ObliqueSlicerBase.initializeTextures();
	m_ObliqueSlicerBase.initializeShader(RENDERING_RESOURCES_DIR "/ogl/SlicerVertex.glsl", RENDERING_RESOURCES_DIR "/ogl/SlicerFragment.glsl");
	m_ObliqueSlicerBase.initializeComputeShader(RENDERING_RESOURCES_DIR "/ogl/ObliqueSliceCompute.glsl");
	m_ObliqueSlicerBase.initializeCrosshair();
}

void ObliqueSlicerWidget::resizeGL(int w, int h)
{
	m_ObliqueSlicerBase.updateSlicerDimensions(w, h);
}

void ObliqueSlicerWidget::paintGL()
{
	//m_SlicerBase.updateSliceSelect(0);
	m_ObliqueSlicerBase.draw(SlicerBase::OBLIQUE);
}

void ObliqueSlicerWidget::mousePressEvent(QMouseEvent *e)
{
	m_ObliqueSlicerBase.mouseOnClick(QVector2D(e->pos()), e->button());

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
		m_ObliqueSlicerBase.cursorOn();
		updateVoxelInfo();
		//sendSliceSelect(m_SlicerBase.getSlicesToSend());
		break;
	default:
		m_MouseLeftClick = false;
		m_MouseRightClick = false;
		break;
	}
	update();
}

void ObliqueSlicerWidget::mouseMoveEvent(QMouseEvent *e)
{
	// TODO: Add support so that SlicerBase sends the Signal rather than getting from Base and sending through sendSliceSelect
	// Using mouse flags allows the other slices to be updated in real time.
	if (m_MouseLeftClick)
	{
		m_ObliqueSlicerBase.mouseOnMove(QVector2D(e->pos()));
	}
	else if (m_MouseRightClick)
	{
		m_ObliqueSlicerBase.mouseOnMove(QVector2D(e->pos()));
		updateVoxelInfo();
	}
	else
	{
		// do nothing
	}
	update();
}

void ObliqueSlicerWidget::mouseReleaseEvent(QMouseEvent *e)
{
	m_ObliqueSlicerBase.mouseOnRelease();
	m_MouseLeftClick = false;
	m_MouseRightClick = false;
	update();
}

void ObliqueSlicerWidget::wheelEvent(QWheelEvent *e)
{
	m_ObliqueSlicerBase.mouseOnWheel(e->angleDelta().y());
	update();
}

void ObliqueSlicerWidget::keyPressEvent(QKeyEvent *e)
{
	m_ObliqueSlicerBase.keyOnPress(e->key());

	switch (e->key())
	{
	case Qt::Key_Escape:
		exit(0);
		break;
	case Qt::Key_W:
		break;
	case Qt::Key_S:
		break;
	default:
		break;
	}

	update();
	e->accept();  // Don't pass any key events to parent
}

void ObliqueSlicerWidget::keyReleaseEvent(QKeyEvent *e)
{
	m_ObliqueSlicerBase.keyOnRelease(e->key());
	
	switch (e->key())
	{
	case Qt::Key_Shift:
		break;
	default:
		break;
	}

	update();
	e->accept();
}

void ObliqueSlicerWidget::updateVoxelInfo()
{
	// To select voxel from CT Data
	int xVox = 0;
	int yVox = 0;
	int zVox = 0;
	m_ObliqueSlicerBase.getOrbitCenter(xVox, yVox, zVox);

	// Send information to SlicerMain for display
	emit sendVoxelInfo(QVector4D(xVox, yVox, zVox, 0));
}
