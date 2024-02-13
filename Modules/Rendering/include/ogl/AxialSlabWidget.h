#ifndef AXIALSLABWIDGET_H
#define AXIALSLABWIDGET_H

#include "MainWindow.h"
#include "config/configure.h"
#include "CTVolume.h"
#include "Camera6DoF.h"
#include "mmUtilityTools.h"
#include "SlabBase.h"

#include "QKeyEvent"

#include "QOpenGLWidget"

class AxialSlabWidget : public QOpenGLWidget
{
	Q_OBJECT

/* METHODS */
public:
	AxialSlabWidget(CTVolume *CTData, MainWindow *mw);
	AxialSlabWidget(CTVolume *CTData, QWidget *parent = 0);
	~AxialSlabWidget();

private:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void keyPressEvent(QKeyEvent *e);



/* VARIABLES */
public:
private:
	MainWindow* m_MainWindow;
	CTVolume *m_CTData;

	GLint m_AxialSliceSelect;
	bool m_MouseLeftClick;
	bool m_MouseRightClick;

	// SlicerBase
	SlabBase m_SlicerBase;
};




#endif // AXIALSLABWIDGET_H