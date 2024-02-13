#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "glad/glad.h"
#include "mmOpenGLErrorCheck.h"

#include <QMainWindow>
#include <QGridLayout>

QT_FORWARD_DECLARE_CLASS(QOpenGLWidget)

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	//void addNew();
	//bool timerEnabled() const { return m_timer->isActive(); }

	//void resizeEvent(QResizeEvent *);

private slots:
	//void updateIntervalChanged(int value);
	//void timerUsageChanged(bool enabled);

private:
	//QTimer * m_timer;
	QGridLayout *m_layout;
	//int m_nextX;
	//int m_nextY;
	//QVector<QOpenGLWidget *> m_glWidgets;
};

#endif