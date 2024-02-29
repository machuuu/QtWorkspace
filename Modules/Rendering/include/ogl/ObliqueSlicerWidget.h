#ifndef OBLIQUESLICERWIDGET_H
#define OBLIQUESLICERWIDGET_H

#include "MainWindow.h"
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
	ObliqueSlicerWidget(CTVolume *CTData, MainWindow *mw);
	ObliqueSlicerWidget(CTVolume *CTData, QWidget *parent = 0);
	~ObliqueSlicerWidget();

signals:
	void sendVoxelInfo(QVector4D voxelInfo);
	void clearVoxelInfo();

public slots:
	void receiveVoxelInfo(QVector4D voxelInfo);

private:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

	void updateVoxelInfo();

	/* VARIABLES */
public:
private:
	MainWindow* m_MainWindow;
	CTVolume *m_CTData;

	//Camera6DoF m_Camera;

	//GLint m_AxialSliceSelect;
	bool m_MouseLeftClick;
	bool m_MouseRightClick;

	// SlicerBase
	ObliqueSlicerBase m_ObliqueSlicerBase;
};




#endif //OBLIQUESLICERWIDGET_H