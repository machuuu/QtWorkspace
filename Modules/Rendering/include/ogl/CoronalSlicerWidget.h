#ifndef CORONALSLICERWIDGET_H
#define CORONALSLICERWIDGET_H

#include "MainWindow.h"
#include "CTVolume.h"
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


// This class shows the Coronal Slices from a 3D CT Image
// The shows the X-Z plane of the CT Image where X is along the screen's x-axis and Z is along the screen's y-axis.
class CoronalSlicerWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	CoronalSlicerWidget(CTVolume *CTData, MainWindow *mw);
	CoronalSlicerWidget(CTVolume *CTData, QWidget *parent = 0);
	~CoronalSlicerWidget();

signals:
	void coronalSliceSelect(QVector2D sliceSelectLocation);
	void sendVoxelInfo(QVector4D voxelInfo);

public slots:
	void axialChangeSlice(QVector2D sliceSelectLocation);
	void sagittalChangeSlice(QVector2D sliceSelectLocation);

private:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void sendSliceSelect(QVector2D sliceSelectLocation);

	QSize getWorkGroups(int workGroupSize);

	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);

private:
	MainWindow* m_MainWindow;
	CTVolume *m_CTData;

	GLint m_CoronalSliceSelect;
	bool m_MouseLeftClick;
	bool m_MouseRightClick;
	
	// SlicerBase
	SlicerBase m_SlicerBase;
};

#endif // CORONALSLICERWIDGET_H