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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
    connect(ui->actionZoom_In, &QAction::triggered, this, &MainWindow::onZoomIn);
    connect(ui->actionZoom_Out, &QAction::triggered, this, &MainWindow::onZoomOut);
    connect(ui->actionReset_Splitter, &QAction::triggered, this, &MainWindow::onResetSplitter);
    connect(ui->splitter, &QSplitter::splitterMoved, this, &MainWindow::onFitToWindow);
        
    ui->actionZoom_In->setEnabled(false);
    ui->actionZoom_Out->setEnabled(false);
    ui->actionNormal_Size->setEnabled(false);
    ui->actionFit_to_Window->setEnabled(false);

    if (displayScene != nullptr) { delete displayScene; displayScene = nullptr; }
    displayScene = new QGraphicsScene();
    displayScene->addText("Click File > Open to select an image to display.");

    ui->graphicsView->setScene(displayScene);

    if (processedScene != nullptr) { delete processedScene; processedScene = nullptr; }
    processedScene = new QGraphicsScene();
    processedScene->addText("Click Edit and select an image processing algorithm to perform on input image.");

    ui->graphicsViewProcessed->setScene(processedScene);
}

void MainWindow::onOpen()
{
    QString filePath;
    
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg *.bmp)"));
    if (dialog.exec()) {
        filePath = dialog.selectedFiles().at(0);
    }

    QImageReader reader(filePath);
    const QImage newImage = reader.read();
    if (newImage.isNull())
    {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
            tr("Cannot load %1: %2")
            .arg(QDir::toNativeSeparators(filePath), reader.errorString()));

        return;
    }

    if (displayImage != nullptr) { delete displayImage; displayImage = nullptr; }
    displayImage = new QImage(newImage);  
    scaleFactor = 1.0;
    
    if (displayScene != nullptr) { delete displayScene; displayScene = nullptr; }
    displayScene = new QGraphicsScene();
    displayScene->addPixmap(QPixmap::fromImage(*displayImage));

    ui->graphicsView->setScene(displayScene);

    if (processedImage != nullptr) { delete processedImage; processedImage = nullptr; }
    processedImage = new QImage(newImage);
    processedScaleFactor = 1.0;

    if (processedScene != nullptr) { delete processedScene; processedScene = nullptr; }
    processedScene = new QGraphicsScene();
    processedScene->addPixmap(QPixmap::fromImage(*processedImage));

    ui->graphicsViewProcessed->setScene(processedScene);

    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    ui->actionNormal_Size->setEnabled(true);
    ui->actionFit_to_Window->setEnabled(true);    

    onFitToWindow();
}


MainWindow::~MainWindow() 
{
    delete ui;
    
    if (displayScene != nullptr) { delete displayScene; displayScene = nullptr; }
    if (displayImage != nullptr) { delete displayImage; displayImage = nullptr; }
    if (processedScene != nullptr) { delete processedScene; processedScene = nullptr; }
    if (processedImage != nullptr) { delete processedImage; processedImage = nullptr; }
}

void MainWindow::onFitToWindow()
{
    // Don't bother to fit to window if the display image does not exist
    if (displayImage != nullptr && displayScene != nullptr)
    {
        // Scene is a simple image, so just need to fit to the rectange of the image
        ui->graphicsView->fitInView(displayScene->sceneRect(), Qt::KeepAspectRatio);

        // Get the current scale once it has been fit to the window
        // Note: both x and y have been scaled together, so grabbing (1,1) as overall scale factor
        scaleFactor = ui->graphicsView->transform().m11();
    }
    
    // Don't bother to fit to window if the processed image does not exist
    if (processedImage != nullptr && processedImage != nullptr)
    {
        // Scene is a simple image, so just need to fit to the rectange of the image
        ui->graphicsViewProcessed->fitInView(processedScene->sceneRect(), Qt::KeepAspectRatio);

        // Get the current scale once it has been fit to the window
        // Note: both x and y have been scaled together, so grabbing (1,1) as overall scale factor
        processedScaleFactor = ui->graphicsViewProcessed->transform().m11();
    }
}

void MainWindow::onZoomIn()
{
    if (displayImage != nullptr && displayScene != nullptr) 
    {
        scaleFactor = (scaleFactor * 1.25 < 3 ? scaleFactor * 1.25 : 3);
        ui->graphicsView->setTransform(QTransform::fromScale(scaleFactor, scaleFactor));
    }

    // Piggy back the processed image based on the existance of display image
    if (processedImage != nullptr && processedImage != nullptr)
    {
        processedScaleFactor = (processedScaleFactor * 1.25 < 3 ? processedScaleFactor * 1.25 : 3);
        ui->graphicsViewProcessed->setTransform(QTransform::fromScale(processedScaleFactor, processedScaleFactor));
    }
    
}

void MainWindow::onZoomOut()
{
    if (displayImage != nullptr && displayScene != nullptr)
    {
        scaleFactor = (scaleFactor * 0.8 > 0.333 ? scaleFactor * 0.8 : 0.333);
        ui->graphicsView->setTransform(QTransform::fromScale(scaleFactor, scaleFactor));
    }

    // Piggy back the processed image based on the existance of display image
    if (processedImage != nullptr && processedImage != nullptr)
    {
        processedScaleFactor = (processedScaleFactor * 0.8 > 0.333 ? processedScaleFactor * 0.8 : 0.333);
        ui->graphicsViewProcessed->setTransform(QTransform::fromScale(processedScaleFactor, processedScaleFactor));
    }
}

void MainWindow::onNormalSize()
{
    if (displayImage != nullptr && displayScene != nullptr)
    {
        scaleFactor = 1.0;
        ui->graphicsView->setTransform(QTransform::fromScale(scaleFactor, scaleFactor));
    }

    // Piggy back the processed image based on the existance of display image
    if (processedImage != nullptr && processedImage != nullptr)
    {
        processedScaleFactor = 1.0;
        ui->graphicsViewProcessed->setTransform(QTransform::fromScale(scaleFactor, scaleFactor));
    }
}

void MainWindow::onResetSplitter()
{
    // Horizontal splitter
    int largeWidth = QGuiApplication::primaryScreen()->virtualSize().width();
    ui->splitter->setSizes(QList<int>({ largeWidth , largeWidth }));
    onFitToWindow();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    onFitToWindow();
}