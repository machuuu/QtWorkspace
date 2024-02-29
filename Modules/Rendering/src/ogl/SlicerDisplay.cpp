#include "SlicerDisplay.h"
#include <QSpacerItem>
#include <QSpinBox>

SlicerDisplay::SlicerDisplay(QOpenGLWidget *slicerWidget, float width, float height) :
	m_arWidth(width),
	m_arHeight(height)
{
	//slicerWidget->setParent(this);

	m_MainLayout = new QGridLayout(this);
	m_MainLayout->setContentsMargins(0, 0, 0, 0);
	
	m_SlicerLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	m_SlicerLayout->addItem(new QSpacerItem(0, 0));
	m_SlicerLayout->addWidget(slicerWidget);
	m_SlicerLayout->addItem(new QSpacerItem(0, 0));

	m_MainLayout->addLayout(m_SlicerLayout, 0, 0);

	m_DisplayLayout = new QBoxLayout(QBoxLayout::LeftToRight);

	int textHeight = 20;
	m_xVoxel = 0;
	m_yVoxel = 0;
	m_zVoxel = 0;
	m_HUValue = 0;
	
	m_xLabel = new QLabel();
	m_xLabel->setText("X: ");
	m_xLabel->setFixedHeight(textHeight);

	m_xLabelValue = new QLabel();
	m_xLabelValue->setText("");
	m_xLabelValue->setFixedHeight(textHeight);

	m_yLabel = new QLabel();
	m_yLabel->setText("Y: ");
	m_yLabel->setFixedHeight(textHeight);

	m_yLabelValue = new QLabel();
	m_yLabelValue->setText("");
	m_yLabelValue->setFixedHeight(textHeight);

	m_zLabel = new QLabel();
	m_zLabel->setText("Z: ");
	m_zLabel->setFixedHeight(textHeight);

	m_zLabelValue = new QLabel();
	m_zLabelValue->setText("");
	m_zLabelValue->setFixedHeight(textHeight);

	m_huLabel = new QLabel();
	m_huLabel->setText("HU: ");
	m_huLabel->setFixedHeight(textHeight);

	m_huLabelValue = new QLabel();
	m_huLabelValue->setText("");
	m_huLabelValue->setFixedHeight(textHeight);

	m_DisplayLayout->addStretch();
	m_DisplayLayout->addWidget(m_xLabel);
	m_DisplayLayout->addWidget(m_xLabelValue);
	m_DisplayLayout->addWidget(m_yLabel);
	m_DisplayLayout->addWidget(m_yLabelValue);
	m_DisplayLayout->addWidget(m_zLabel);
	m_DisplayLayout->addWidget(m_zLabelValue);
	m_DisplayLayout->addWidget(m_huLabel);
	m_DisplayLayout->addWidget(m_huLabelValue);
	m_DisplayLayout->addStretch();

	m_MainLayout->addLayout(m_DisplayLayout, 1, 0);

	resize(width,height);

	//resize(width, height);

	QObject::connect(slicerWidget, SIGNAL(sendVoxelInfo(QVector4D)), this, SLOT(updateVoxelInfo(QVector4D)));
	QObject::connect(slicerWidget, SIGNAL(clearVoxelInfo()), this, SLOT(onClearVoxelInfo()));
}


SlicerDisplay::~SlicerDisplay()
{

}

void SlicerDisplay::resizeEvent(QResizeEvent *e)
{
	float thisAspectRatio = static_cast<float>(e->size().width()) / static_cast<float>(e->size().height());
	int widgetStretch, outerStretch;

	if (thisAspectRatio > (m_arWidth / m_arHeight))
	{
		m_SlicerLayout->setDirection(QBoxLayout::LeftToRight);
		widgetStretch = height() * (m_arWidth / m_arHeight);
		outerStretch = (width() - widgetStretch) / 2.0 + 0.5;
	}
	else
	{
		m_SlicerLayout->setDirection(QBoxLayout::TopToBottom);
		widgetStretch = width() * (m_arHeight / m_arWidth);
		outerStretch = (height() - widgetStretch) / 2.0 + 0.5;
	}
	m_SlicerLayout->setStretch(0, outerStretch);
	m_SlicerLayout->setStretch(1, widgetStretch);
	m_SlicerLayout->setStretch(2, outerStretch);
	//resizeSlicerLayout();
}



void SlicerDisplay::resizeSlicerLayout(int w, int h)
{
	//float width = static_cast<float>(w);
	//float height =static_cast<float>(h);
	//
	//float thisAspectRatio = width / height;
	//int widgetStretch, outerStretch;
	//
	//if (thisAspectRatio > (m_arWidth / m_arHeight))
	//{
	//	m_SlicerLayout->setDirection(QBoxLayout::LeftToRight);
	//	widgetStretch = height * (m_arWidth / m_arHeight);
	//	outerStretch = (width - widgetStretch) / 2.0 + 0.5;
	//}
	//else
	//{
	//	m_SlicerLayout->setDirection(QBoxLayout::TopToBottom);
	//	widgetStretch = width * (m_arHeight / m_arWidth);
	//	outerStretch = (height - widgetStretch) / 2.0 + 0.5;
	//}
	//m_SlicerLayout->setStretch(0, outerStretch);
	//m_SlicerLayout->setStretch(1, widgetStretch);
	//m_SlicerLayout->setStretch(2, outerStretch);
}

void SlicerDisplay::updateVoxelInfo(QVector4D voxelInfo)
{
	m_xLabelValue->setNum(voxelInfo.x());
	m_yLabelValue->setNum(voxelInfo.y());
	m_zLabelValue->setNum(voxelInfo.z());
	m_huLabelValue->setNum(voxelInfo.w());
	this->update();
}

void SlicerDisplay::onClearVoxelInfo()
{
	m_xLabelValue->setText("");
	m_yLabelValue->setText("");
	m_zLabelValue->setText("");
	m_huLabelValue->setText("");
	this->update();
}

