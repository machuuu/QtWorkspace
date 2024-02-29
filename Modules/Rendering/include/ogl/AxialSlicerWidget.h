#ifndef AXIALSLICERWIDGET_H
#define AXIALSLICERWIDGET_H

#include "MainWindow.h"
#include "CTVolume.h"
#include "Camera6DoF.h"
#include "mmUtilityTools.h"
#include "SlicerBase.h"

#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QSize>
#include <QVector4D>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>



// This class shows the Axial Slices from a 3D CT Image
// The shows the X-Y plane of the CT Image where X is along the screen's x-axis and Y is along the screen's y-axis.
class AxialSlicerWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	AxialSlicerWidget(CTVolume *CTData, MainWindow *mw);
	AxialSlicerWidget(CTVolume *CTData, QWidget *parent = 0);
	~AxialSlicerWidget();

signals:
	void axialSliceSelect(QVector2D sliceSelectLocation);
	void sendVoxelInfo(QVector4D voxelInfo);

public slots:
	void sagittalChangeSlice(QVector2D sliceSelectLocation);
	void coronalChangeSlice(QVector2D sliceSelectLocation);

private:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;
	
	void sendSliceSelect(QVector2D sliceSelectLocation);

	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *e) ;
	void mouseMoveEvent(QMouseEvent *e) ;
	void mouseReleaseEvent(QMouseEvent *e) ;
	void wheelEvent(QWheelEvent *e);

private:
	MainWindow* m_MainWindow;
	CTVolume *m_CTData;

	GLint m_AxialSliceSelect;
	bool m_MouseLeftClick;
	bool m_MouseRightClick;

	// SlicerBase
	SlicerBase m_SlicerBase;
};

#endif // AXIALSLICERWIDGET_H