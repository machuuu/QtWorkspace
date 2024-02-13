#ifndef AXIALSLICERWIDGET_H
#define AXIALSLICERWIDGET_H

#include "MainWindow.h"
#include "configure.h"
#include "CTVolume.h"
#include "Camera6DoF.h"
#include "SlicerHelper.h"
#include "Crosshair.h"

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

	enum UpdateType
	{
		fromPan,
		fromZoom
	};

	enum WindowType
	{
		LUNG,
		MEDIASTINUM,
		XSECTION
	};

signals:
	void axialSliceSelect(QVector2D sliceSelectLocation);
	void sendVoxelInfo(QVector4D voxelInfo);

public slots:
	// void baseAxialSliceSelect(QVector2D sliceSelectLocation);
	void sagittalChangeSlice(QVector2D sliceSelectLocation);
	void coronalChangeSlice(QVector2D sliceSelectLocation);

private:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void initTexture();
	void initTextureGeometry();
	void initTextureShaders();
	
	void sendSliceSelect(QVector2D sliceSelectLocation);

	void getWindowParameters(float &windowLevel, float &windowWidth, WindowType select);

	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *e) ;
	void mouseMoveEvent(QMouseEvent *e) ;
	void mouseReleaseEvent(QMouseEvent *e) ;
	void wheelEvent(QWheelEvent *e);

	void updateProjectionMatrix(UpdateType typeSelect);
	void constructProjectionMatrix();
	
	void drawTextureGeometry();


private:
	MainWindow* m_MainWindow;
	
	/* MVP Matrices */
	glm::mat4 m_Projection;
	glm::mat4 m_View;
	glm::mat4 m_Model;
	glm::mat4 m_Scale;

	/* Slice Selection */
	int m_ScreenWidth;
	int m_ScreenHeight;

	/* Camera */
	Camera6DoF m_Camera;
	bool m_MouseLeftClick;
	bool m_MouseRightClick;
	QVector2D m_MouseLastPosition;
	float m_Zoom;
	glm::vec4 m_OrthoProjPos; // Left|Right|Bottom|Top
	float m_ZoomSensitivity;

	// Texture
	CTVolume *m_CTData;
	GLfloat m_Vertices[16];
	GLuint m_Indicies[6];
	GLuint m_CTTexture;
	GLuint m_SliceTexture;
	GLuint m_ColormapTexture;
	GLuint m_VertexArray;
	GLuint m_VertexBuffer;
	GLuint m_IndexBuffer;
	GLint m_AxialSliceSelect;
	glm::vec4 m_TextureBounds; // Left|Right|Bottom|Top

	// Crosshair
	//GLfloat m_CrosshairVertices[8];
	//GLuint m_CrosshairVertexArray;
	//GLuint m_CrosshairVertexBuffer;
	//GLuint m_CrosshairShaderProgram;
	//glm::mat4 m_CrosshairModelOffsest;
	Crosshair m_Crosshair;

		
	// Shader Program
	GLuint m_SlicerShaderProgram;
	GLuint m_SlicerComputeProgram;
};

#endif // AXIALSLICERWIDGET_H