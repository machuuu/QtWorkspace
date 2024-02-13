#include "ObliqueSlicerWidget.h"

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

void ObliqueSlicerWidget::initializeGL()
{
	if (gladLoadGL())
		std::cout << "GLAD Loaded" << std::endl;
	else
		std::cout << "Error Loading GLAD" << std::endl;

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	m_SlicerBase = ObliqueSlicerBase(m_CTData->getNumColumns(), m_CTData->getNumRows(), m_CTData->getNumSlices(), width(), height(), m_CTData);


	
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


	m_SlicerBase.initializeTextures();
	m_SlicerBase.initializeShader(SHADER_DIR "/SlicerVertex.glsl", SHADER_DIR "/SlicerFragment.glsl");
	m_SlicerBase.initializeComputeShader(SHADER_DIR "/ObliqueSliceCompute.glsl");
	m_SlicerBase.initializeCrosshair();
}

void ObliqueSlicerWidget::resizeGL(int w, int h)
{
	m_SlicerBase.updateSlicerDimensions(w, h);
}

void ObliqueSlicerWidget::paintGL()
{
	//m_SlicerBase.updateSliceSelect(0);
	m_SlicerBase.draw(SlicerBase::OBLIQUE);
}

void ObliqueSlicerWidget::mousePressEvent(QMouseEvent *e)
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
		m_SlicerBase.mouseOnMove(QVector2D(e->pos()));
	}
	else if (m_MouseRightClick)
	{
		//m_Crosshair.updatePosition(QVector2D(e->pos()), width(), height());
		//sendSliceSelect(m_SlicerBase.getSlicesToSend());
		m_SlicerBase.mouseOnMove(QVector2D(e->pos()));
	}
	else
	{
		// do nothing
	}
	update();
}

void ObliqueSlicerWidget::mouseReleaseEvent(QMouseEvent *e)
{
	m_SlicerBase.mouseOnRelease();
	m_MouseLeftClick = false;
	m_MouseRightClick = false;
	update();
}

void ObliqueSlicerWidget::wheelEvent(QWheelEvent *e)
{
	m_SlicerBase.mouseOnWheel(e->delta());
	update();
}

void ObliqueSlicerWidget::keyPressEvent(QKeyEvent *e)
{
	m_SlicerBase.keyOnPress(e->key());

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
	m_SlicerBase.keyOnRelease(e->key());
	
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
