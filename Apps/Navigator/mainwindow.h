#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class AirwayRenderWidget;
class QGridLayout;
class QLabel;
class QPushButton;
class AxialSlicerWidget;
class CoronalSlicerWidget;
class SagittalSlicerWidget;
class SlicerDisplay;
class CTVolume;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    //void OnOpen();

private slots:
	void dialogAirwaySurface();
	void dialogAnalyzeImage();


    // void onFitToWindow();
    // void onZoomIn();
    // void onZoomOut();
    // void onNormalSize();
    // void onResetSplitter();

protected:
    // void resizeEvent(QResizeEvent* event) override;

private:

    void loadAirwaySurface(const std::string& filepath);
    void loadAnalzeImage(const std::string& filepath);

    Ui::MainWindow *ui;
    CTVolume* ctVolume = nullptr;

};
#endif // MAINWINDOW_H
