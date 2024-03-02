#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "glad/glad.h"
#include "mmOpenGLErrorCheck.h"
#include <vector>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <mmUtilityTools.h>


#include <QVector2D>

class Crosshair
{
public:
	Crosshair();
	~Crosshair();

	Crosshair(const Crosshair&) = delete;
	Crosshair(Crosshair&&) = delete;
	Crosshair& operator=(const Crosshair&) = delete;
	Crosshair& operator=(Crosshair&&) = delete;

	void draw();
	void updatePosition(QVector2D mouseLocation, int screenWidth, int screenHeight);
	void init(glm::vec2 center);

	void getPosition();

private:
	void initGeometry(glm::vec2 center);
	void initShaders();

private:

	std::vector<GLfloat> m_Vertices;
	GLuint m_VertexArray;
	GLuint m_VertexBuffer;
	GLuint m_ShaderProgram;

	glm::vec2 m_Center;
	glm::mat4 m_ModelOffset;


};



#endif // CROSSHAIR_H