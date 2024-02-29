#include "Crosshair.h"
#include "rendering.configure.h"

Crosshair::Crosshair()
{
	//m_Center = glm::vec2(0.0f, 0.0f);
	m_ModelOffset = glm::mat4(1.0f);
	//initGeometry(m_Center);
	//initShaders();
}

Crosshair::~Crosshair()
{
	glDeleteVertexArrays(1, &m_VertexArray);
	glDeleteBuffers(1, &m_VertexBuffer);
}

void Crosshair::draw()
{
	mmOpenGLClearErrorMacro();

	glUseProgram(m_ShaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "model"), 1, GL_FALSE, &m_ModelOffset[0][0]);

	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glDrawArrays(GL_LINES, 0, 8);

	glUseProgram(0);
	mmOpenGLCheckErrorMacro("Error with drawing crosshair - OpenGL");
}

void Crosshair::initGeometry(glm::vec2 center)
{
	m_Vertices.clear();
	// start it far off screen
	m_Vertices = {
		// Center to Right
		0.0f,  0.0f,  0.0f,
		5.0f,  0.0f,  0.0f,
		// Center to Up
		0.0f,  0.0f,  0.0f,
		0.0f,  5.0f,  0.0f,
		// Center to Left
		0.0f,  0.0f,  0.0f,
		-5.0f,  0.0f,  0.0f,
		// Center to Down
		0.0f,  0.0f,  0.0f,
		0.0f, -5.0f,  0.0f
	};

	mmOpenGLClearErrorMacro();

	glGenVertexArrays(1, &m_VertexArray);
	glBindVertexArray(m_VertexArray);

	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(GLfloat), &m_Vertices[0], GL_STATIC_DRAW);

	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(0));
	glEnableVertexAttribArray(0);

	mmOpenGLCheckErrorMacro("Error with initializing crosshair geometry - OpenGL");
}

void Crosshair::initShaders()
{
	m_ShaderProgram = mm::getShaderProgram(RENDERING_RESOURCES_DIR "/ogl/CrosshairVertex.glsl", RENDERING_RESOURCES_DIR "/ogl/CrosshairFragment.glsl");
}

void Crosshair::updatePosition(QVector2D mouseLocation, int screenWidth, int screenHeight)
{
	float xLoc = mouseLocation.x();
	float yLoc = mouseLocation.y();

	float height = static_cast<float>(screenHeight);
	float width = static_cast<float>(screenWidth);

	xLoc = (xLoc - width / 2.0f) / (width / 2.0f);
	yLoc = -((yLoc - height / 2.0f) / (height / 2.0f)); //invert yLoc because mouseLocation is inverted to OpenGL ScreenSpace

	m_ModelOffset = glm::translate(glm::mat4(1.0f), glm::vec3(xLoc, yLoc, 0.0f));
}

void Crosshair::init(glm::vec2 center)
{
	m_Center = center;
	initGeometry(m_Center);
	initShaders();
}

