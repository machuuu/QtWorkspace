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

#include <filesystem>

#include "CTVolume.h"
#include "AxialSlicerWidget.h"
#include "CoronalSlicerWidget.h"
#include "SagittalSlicerWidget.h"
#include "SlicerDisplay.h"
#include "ObliqueSlicerWidget.h"

#include "Logger.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(ui->actionOpenAirwaySurface, &QAction::triggered, this, &MainWindow::dialogAirwaySurface);
	connect(ui->actionOpenAnalyzeImage, &QAction::triggered, this, &MainWindow::dialogAnalyzeImage);

	connect(ui->pushButtonAirwaySurface, &QPushButton::clicked, this, &MainWindow::dialogAirwaySurface);
	connect(ui->pushButtonAnalyzeImage, &QPushButton::clicked, this, &MainWindow::dialogAnalyzeImage);

	ui->airwaySurfaceWidget->hide();
	ui->analyzeImageWidget->hide();

#ifdef QTWS_DEVELOPER
	loadAirwaySurface(QTWS_DATA_DIR "/1.3.12.2.1107.5.1.4.67126.30000018101712091196200008839_blendedpolygons.ply");
	loadAnalzeImage(QTWS_DATA_DIR "/1.3.12.2.1107.5.1.4.67126.30000018101712091196200008839_analyze.hdr");
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
		auto airwayRenderWidget = new AirwayRenderWidget(filepath);
		//airwayRenderWidget->setMinimumHeight(300);
		airwayRenderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->addWidget(airwayRenderWidget);

		ui->airwaySurfaceWidget->setLayout(hLayout);
		//ui->airwaySurfaceWidget->setMinimumHeight(300);

		ui->loadAirwaySurfaceWidget->hide();
		ui->airwaySurfaceWidget->show();
	}
}

void MainWindow::loadAnalzeImage(const std::string& headerFilePath)
{
	if (!headerFilePath.empty())
	{
		if (ctVolume != nullptr) { delete ctVolume; ctVolume = nullptr; }

		std::filesystem::path imageFilePath(headerFilePath);
		imageFilePath.replace_extension(".img");
		ctVolume = new CTVolume(headerFilePath, imageFilePath.string());

		auto axialSlicerWidget = new AxialSlicerWidget(ctVolume);
		auto coronalSlicerWidget = new CoronalSlicerWidget(ctVolume);
		auto sagittalSlicerWidget = new SagittalSlicerWidget(ctVolume);

		QObject::connect(axialSlicerWidget, SIGNAL(axialSliceSelect(QVector2D)), sagittalSlicerWidget, SLOT(axialChangeSlice(QVector2D)));
		QObject::connect(axialSlicerWidget, SIGNAL(axialSliceSelect(QVector2D)), coronalSlicerWidget, SLOT(axialChangeSlice(QVector2D)));

		QObject::connect(coronalSlicerWidget, SIGNAL(coronalSliceSelect(QVector2D)), sagittalSlicerWidget, SLOT(coronalChangeSlice(QVector2D)));
		QObject::connect(coronalSlicerWidget, SIGNAL(coronalSliceSelect(QVector2D)), axialSlicerWidget, SLOT(coronalChangeSlice(QVector2D)));

		QObject::connect(sagittalSlicerWidget, SIGNAL(sagittalSliceSelect(QVector2D)), coronalSlicerWidget, SLOT(sagittalChangeSlice(QVector2D)));
		QObject::connect(sagittalSlicerWidget, SIGNAL(sagittalSliceSelect(QVector2D)), axialSlicerWidget, SLOT(sagittalChangeSlice(QVector2D)));

		auto axialSlicerDisplay = new SlicerDisplay(axialSlicerWidget, 300, (300 / ctVolume->getAxialAspectRatio()));
		//axialSlicerDisplay->setMinimumHeight(250);
		axialSlicerDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		auto coronalSlicerDisplay = new SlicerDisplay(coronalSlicerWidget, 300, (300 / ctVolume->getCoronalAspectRatio()));
		//coronalSlicerDisplay->setMinimumHeight(250);
		coronalSlicerDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		auto sagittalSlicerDisplay = new SlicerDisplay(sagittalSlicerWidget, 300, (300 / ctVolume->getSagittalAspectRatio()));
		//sagittalSlicerDisplay->setMinimumHeight(250);
		sagittalSlicerDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		auto obliqueSlicerWidget = new ObliqueSlicerWidget(ctVolume);
		auto obliqueSlicerDisplay = new SlicerDisplay(obliqueSlicerWidget, 300, 300);
		obliqueSlicerDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		//obliqueSlicerDisplay->setMinimumHeight(250);

		QGridLayout* gridLayout = new QGridLayout();
		gridLayout->addWidget(axialSlicerDisplay, 0, 0);
		gridLayout->addWidget(coronalSlicerDisplay, 0, 1);
		gridLayout->addWidget(sagittalSlicerDisplay, 0, 2);
		gridLayout->addWidget(obliqueSlicerDisplay, 1, 1);
		gridLayout->setColumnStretch(0, 1);
		gridLayout->setColumnStretch(1, 1);
		gridLayout->setColumnStretch(2, 1);
		ui->analyzeImageWidget->setLayout(gridLayout);
		//ui->analyzeImageWidget->setMinimumHeight(600);

		ui->loadAnalyzeImageWidge->hide();
		ui->analyzeImageWidget->show();
	}
}

MainWindow::~MainWindow() 
{
    delete ui;
	if (ctVolume != nullptr) { delete ctVolume; ctVolume = nullptr; }
	// Qt's QObject parent/child will clean up resources
}