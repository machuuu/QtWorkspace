#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qfiledialog.h>
#include <qlabel.h>
#include <qevent.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qimagereader.h>
#include <qtransform.h>
#include <qmessagebox.h>

#include "AirwayRenderWidget.h"
#include "QPushButton"
#include "QHBoxLayout"
#include "QVBoxLayout"

#include "configure.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(ui->actionOpenAirwaySurface, &QAction::triggered, this, &MainWindow::dialogAirwaySurface);
	connect(ui->actionOpenAnalyzeImage, &QAction::triggered, this, &MainWindow::dialogAnalyzeImage);

	if (loadAirwaySurfaceButton != nullptr) { delete loadAirwaySurfaceButton; loadAirwaySurfaceButton = nullptr; }
	loadAirwaySurfaceButton = new QPushButton("Load Airway Surface");
	//loadAirwaySurfaceButton->setFixedSize(150, 30);
	connect(loadAirwaySurfaceButton, &QPushButton::clicked, this, &MainWindow::dialogAirwaySurface);

	if (loadAnalyzeImageButton != nullptr) { delete loadAnalyzeImageButton; loadAnalyzeImageButton = nullptr; }
	loadAnalyzeImageButton = new QPushButton("Load Analyze Image");
	//loadAnalyzeImageButton->setFixedSize(150, 30);
	connect(loadAnalyzeImageButton, &QPushButton::clicked, this, &MainWindow::dialogAnalyzeImage);

	if (mainGridLayout != nullptr) { delete mainGridLayout; mainGridLayout = nullptr; }

	mainGridLayout = new QGridLayout();
	mainGridLayout->addWidget(loadAirwaySurfaceButton, 0, 0);
	mainGridLayout->addWidget(loadAnalyzeImageButton, 1, 0);

	ui->centralwidget->setLayout(mainGridLayout);

#ifdef QTWS_DEVELOPER
	loadAirwaySurface(QTWS_DATA_DIR "/ROIMT.ply");
	loadAnalzeImage(QTWS_DEVELOPER "/test.hdr");
#endif
}

void MainWindow::dialogAirwaySurface()
{
	QString filePath;

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Airway Tree Surface (*.ply)"));
	if (dialog.exec()) {
		filePath = dialog.selectedFiles().at(0);
	}

	loadAirwaySurface(filePath.toStdString());
}

void MainWindow::dialogAnalyzeImage()
{
	QString filePath;

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Analyze Image (*.hdr)"));
	if (dialog.exec()) {
		filePath = dialog.selectedFiles().at(0);
	}

	loadAnalzeImage(filePath.toStdString());
}

void MainWindow::loadAirwaySurface(const std::string& filepath)
{
	if (!filepath.empty())
	{
		if (airwayRenderWidget != nullptr) { delete airwayRenderWidget; airwayRenderWidget = nullptr; }

		airwayRenderWidget = new AirwayRenderWidget(filepath);

		mainGridLayout->removeWidget(loadAirwaySurfaceButton);
		loadAirwaySurfaceButton->setParent(nullptr); // Keep button for later ... 
		mainGridLayout->addWidget(airwayRenderWidget, 0, 0);
	}
}

void MainWindow::loadAnalzeImage(const std::string& filepath)
{

}

MainWindow::~MainWindow() 
{

	// todo :: what is the proper order of cleaning up? 
	// if another application uses this window, would like to ensure all resources are cleared
	// currently, doesn't really matter because when this is closed, the app will close.

    delete ui;

	if (loadAirwaySurfaceButton != nullptr) { delete loadAirwaySurfaceButton; loadAirwaySurfaceButton = nullptr; }
	if (loadAnalyzeImageButton != nullptr) { delete loadAnalyzeImageButton; loadAnalyzeImageButton = nullptr; }

	if (airwayRenderWidget != nullptr) { delete airwayRenderWidget; airwayRenderWidget = nullptr; }

	if (mainGridLayout != nullptr) { delete mainGridLayout; mainGridLayout = nullptr; }
}