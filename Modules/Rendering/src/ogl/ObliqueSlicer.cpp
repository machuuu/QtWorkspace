#include "ObliqueSlicerWidget.h"

AxialSlabWidget::AxialSlabWidget(CTVolume *CTData, MainWindow *mw) :
	m_MainWindow(mw),
	m_CTData(CTData)
{

}

AxialSlabWidget::AxialSlabWidget(CTVolume *CTData, QWidget *parent /*= 0*/) :
	m_CTData(CTData),
	m_MouseLeftClick(false),
	m_MouseRightClick(false)
{

}

AxialSlabWidget::~AxialSlabWidget()
{

}

void AxialSlabWidget::initializeGL()
{
	if (gladLoadGL())
		std::cout << "GLAD Loaded" << std::endl;
	else
		std::cout << "Error Loading GLAD" << std::endl;

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	m_AxialSliceSelect = 0;

	m_SlicerBase = SlabBase(m_CTData->getNumColumns(), m_CTData->getNumRows(), m_CTData->getNumSlices(), width(), height(), m_CTData);

	m_SlicerBase.initializeTextures();
	m_SlicerBase.initializeShader(SHADER_DIR "/SlicerVertex.glsl", SHADER_DIR "/SlicerFragment.glsl");
	m_SlicerBase.initializeComputeShader(SHADER_DIR "/AxialSlabCompute.glsl");
	m_SlicerBase.initializeCrosshair();
}

void AxialSlabWidget::resizeGL(int w, int h)
{
	m_SlicerBase.updateSlicerDimensions(w, h);
}

void AxialSlabWidget::paintGL()
{
	//m_SlicerBase.updateSliceSelect(0);
	m_SlicerBase.draw(SlabBase::AXIAL);
}

void AxialSlabWidget::keyPressEvent(QKeyEvent *e)
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
