#ifndef SLICERBASE_H
#define SLICERBASE_H

#include "CTVolume.h"
#include "Camera6DoF.h"
#include <SlicerCamera.h>
#include "mmUtilityTools.h"
#include "Crosshair.h"

#include "QVector2D"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// This class is used with the Axial, Sagittal and Coronal Slicer Widgets
// Create this class within each widget
// It contains the required common methods
class SlicerBase
{
/* ENUMS */
public:
	enum WindowType {
		LUNG,
		MEDIASTINUM,
		XSECTION
	};
	enum ColormapSelect {
		GRAYSCALE,
	};
	enum TextureFlipSelect {
		NONE,
		VERTICALLY
	};
	enum InterpolationSelect {
		NEAREST,
		LINEAR
	};
	enum ProjectionUpdateSelect {
		fromPan,
		fromZoom
	};
	enum SlicerSelect {
		AXIAL,
		SAGITTAL,
		CORONAL,
		OBLIQUE
	};
private:

/* METHODS */
public:
	SlicerBase();
	SlicerBase(int slicerXDim, int slicerYDim, int slicerWidth, int slicerHeight, CTVolume* CTData);
	~SlicerBase();

	/* Initializations */
	virtual void initializeTextures();
	void initializeShader(const char* vertexPath, const char* fragmentPath);
	void initializeComputeShader(const char* computePath);
	void initializeCrosshair();
	
	/* Set Functions */
	void updateShaderProgram(const char* vertexPath, const char* fragmentPath);
	void updateComputeShaderProgram(const char* computePath);	
	void updateColorMap(ColormapSelect select);	
	void updateSlicerDimensions(int width, int height);
	void updateSliceSelect(int sliceSelect);
	void updateCrosshairPosition(QVector2D mouseLocation);
	void setCameraVectors(glm::vec3 cameraFront, glm::vec3 cameraRight, glm::vec3 cameraUp);
	void setWindowParameters(WindowType select);

	/* User Input */
	void mouseOnClick(QVector2D mousePosition, Qt::MouseButton mouseButton);
	virtual void mouseOnMove(QVector2D mousePosition);
	void mouseOnRelease();
	void mouseOnWheel(int wheelDelta);
	virtual void keyOnPress(int keySelect);
	void keyOnRelease(int keySelect);

	/* Drawing */
	virtual void draw(SlicerSelect select);
	virtual void computeSliceTexture(SlicerSelect select);
	void drawSliceTexture();
	void drawCrosshair();
	void cursorOn();
	void cursorOff();
	
	/* Get Functions */
	void computeSlicesToSend(QVector2D mousePosition);
	QVector2D getSlicesToSend();
	GLuint getCTTextureID();
	GLuint getSliceTextureID();
	GLuint getColormapTextureID();
	GLuint getBlankTextureID();
	GLuint getVertexArrayID();
	GLuint getVertexBufferID();
	GLuint getIndexArrayID();
	GLuint getShaderProgramID();
	GLuint getComputeProgramID();
	

protected:
	void createTextureQuad(TextureFlipSelect select);
	void createCTVolumeTexture();
	virtual void createSliceTexture();
	void createColorMap(ColormapSelect select);
	void createGrayColorMap(InterpolationSelect select);
	void createShaderProgram(const char* vertexPath, const char* fragmentPath);
	void createComputeShaderProgram(const char* computePath);
	void drawTextureQuad(int vertexCount = 6);

	void createProjectionMatrix();
	void updateProjectionMatrix(ProjectionUpdateSelect select);
	
	GLuint compileShader(GLuint type, const char* shaderPath);
	void checkCompileErrors(GLuint shader, std::string type);

/* VARIABLES */
public:
protected:
	int m_XVoxels;
	int m_YVoxels;
	int m_SlicerWidth;
	int m_SlicerHeight;
	
	float m_WindowLevel;
	float m_WindowWidth;

	GLuint m_CTTexture;
	GLuint m_SliceTexture;
	GLuint m_ColormapTexture;

	GLuint m_VertexArray;
	GLuint m_VertexBuffer;
	GLuint m_IndexBuffer;

	GLuint m_ComputeProgram;
	GLuint m_ShaderProgram;

	GLint m_SliceSelect;

	glm::mat2 m_TextureScale;
	glm::mat4 m_ModelScale;
	glm::mat4 m_Model;
	glm::mat4 m_View;
	glm::mat4 m_Projection;
	glm::vec4 m_OrthoProjPos;

	CTVolume *m_CTData;
	Crosshair m_Crosshair;
	SlicerCamera m_Camera;

	float m_ZoomAmount;
	float m_ZoomSensitivity;

	bool m_MouseLeftClick;
	bool m_MouseRightClick;
	bool m_ShiftSelect;
	bool m_CursorOn;
	
	QVector2D m_MouseLastPosition;
	QVector2D m_SlicesToSend;
};



#endif // SLICERBASE_H