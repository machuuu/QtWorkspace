#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QGraphicsScene;
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
    void onOpen();
    void onFitToWindow();
    void onZoomIn();
    void onZoomOut();
    void onNormalSize();
    void onResetSplitter();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::MainWindow *ui;
    QImage* displayImage = nullptr;
    QGraphicsScene* displayScene = nullptr;
    QImage* processedImage = nullptr;
    QGraphicsScene* processedScene = nullptr;
    
    double scaleFactor = 1.0;
    double processedScaleFactor = 1.0;
};
#endif // MAINWINDOW_H
