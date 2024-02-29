#ifndef SAGITTALSLICERWIDGET_H
#define SAGITTALSLICERWIDGET_H

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

// This class shows the Sagittal Slices from a 3D CT Image
// The shows the Y-Z plane of the CT Image where Y is along the screen's x-axis and Z is along the screen's y-axis.
class SagittalSlicerWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	SagittalSlicerWidget(CTVolume *CTData, MainWindow *mw);
	SagittalSlicerWidget(CTVolume *CTData, QWidget *parent = 0);
	~SagittalSlicerWidget();

	enum UpdateType
	{
		fromPan,
		fromZoom
	};

signals:
	void sagittalSliceSelect(QVector2D sliceSelectLocation);
	void sendVoxelInfo(QVector4D voxelInfo);
	void clearVoxelInfo();

public slots:
	void axialChangeSlice(QVector2D sliceSelectLocation);
	void coronalChangeSlice(QVector2D sliceSelectLocation);
	void receiveVoxelInfo(QVector4D voxelInfo);

private:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void sendSliceSelect(QVector2D sliceSelectLocation);

	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
	
private:
	MainWindow* m_MainWindow;
	CTVolume *m_CTData;

	GLint m_SagittalSliceSelect;
	bool m_MouseLeftClick;
	bool m_MouseRightClick;
	
	SlicerBase m_SlicerBase;
};

#endif // SAGITTALSLICERWIDGET_H