#include "MainWindow.h"
#include "AirwayRenderWidget.h"
#include "AxialSlicerWidget.h"
#include "CoronalSlicerWidget.h"
#include "SagittalSlicerWidget.h"

#include <QGroupBox>


MainWindow::MainWindow()
{	
	// Create AirwayTreeWidget	
	//AirwayRenderWidget *airwayRender = new AirwayRenderWidget(this);
	//airwayRender->setFixedSize(1200, 800);

	// Initialize CTVolume for Slicers
	//std::string headerFilename("X:/MIPL/Cases/20349 Cases/20349.3.90/20349.3.90/20349_3_90_B31f.hdr");
	//std::string imageFilename("X:/MIPL/Cases/20349 Cases/20349.3.90/20349.3.90/20349_3_90_B31f.img");

	std::string headerFilename("X:/MIPL/Cases/20349 Cases/20349.3.91/CT/20349_3_91_B31f.hdr");
	std::string imageFilename("X:/MIPL/Cases/20349 Cases/20349.3.91/CT/20349_3_91_B31f.img");
	
	//CTVolume *CTData = new CTVolume(headerFilename, imageFilename);

	// Create slicers
	//AxialSlicerWidget *axialSlicer = new AxialSlicerWidget(CTData, this);
	//axialSlicer->setFixedSize(400,400);
	//CoronalSlicerWidget *coronalSlicer = new CoronalSlicerWidget(CTData, this);
	//coronalSlicer->setFixedSize(400, 400);
	//SagittalSlicerWidget *sagittalSlicer = new SagittalSlicerWidget(CTData, this);
	//sagittalSlicer->setFixedSize(400, 400);

	QGroupBox *groupBox = new QGroupBox(this);
	setCentralWidget(groupBox);

	m_layout = new QGridLayout(groupBox);

	//m_layout->addWidget(airwayRender, 0, 0, 1, 3);
	//m_layout->addWidget(axialSlicer, 1, 0);
	//m_layout->addWidget(coronalSlicer, 1, 1);
	//m_layout->addWidget(sagittalSlicer, 1, 2);
}