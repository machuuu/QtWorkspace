#ifndef OBLIQUESLICERWIDGET_H
#define OBLIQUESLICERWIDGET_H

#include "MainWindow.h"
#include "config/configure.h"
#include "CTVolume.h"
#include "Camera6DoF.h"
#include "mmUtilityTools.h"
#include "ObliqueSlicerBase.h"

#include "QKeyEvent"

#include "QOpenGLWidget"

class ObliqueSlicerWidget : public QOpenGLWidget
{
	Q_OBJECT

	/* METHODS */
public:
	ObliqueSlicer(CTVolume *CTData, MainWindow *mw);
	ObliqueSlicer(CTVolume *CTData, QWidget *parent = 0);
	~ObliqueSlicer();

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
	ObliqueSlicerBase m_SlicerBase;
};




#endif //OBLIQUESLICERWIDGET_H