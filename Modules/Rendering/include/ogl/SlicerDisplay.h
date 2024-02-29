#ifndef SLICERMAIN_H
#define SLICERMAIN_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QBoxLayout>
#include <QResizeEvent>
#include <QGridLayout>
#include <QVector4D>
#include <QLabel>

// Influenced by: https://stackoverflow.com/questions/30005540/keeping-the-aspect-ratio-of-a-sub-classed-qwidget-during-resize

// This class is used to display a Slicer Widget.
class SlicerDisplay : public QWidget
{
	Q_OBJECT

public:
	SlicerDisplay() = delete;
	SlicerDisplay(QOpenGLWidget *slicerWidget, float width, float height);
	~SlicerDisplay();
	void resizeEvent(QResizeEvent *e);
	void resizeSlicerLayout(int width, int height);

public slots:
	void updateVoxelInfo(QVector4D voxelInfo);

private:
	QOpenGLWidget *m_SlicerWidget;
	QGridLayout *m_MainLayout;
	QBoxLayout *m_SlicerLayout;
	QBoxLayout *m_DisplayLayout;
	float m_arWidth;
	float m_arHeight;

	QLabel *m_xLabel;
	QLabel *m_xLabelValue;
	QLabel *m_yLabel;
	QLabel *m_yLabelValue;
	QLabel *m_zLabel;
	QLabel *m_zLabelValue;
	QLabel *m_huLabel;
	QLabel *m_huLabelValue;	
	
	int m_xVoxel;
	int m_yVoxel;
	int m_zVoxel;
	int m_HUValue;
};

#endif //SLICERMAIN_H