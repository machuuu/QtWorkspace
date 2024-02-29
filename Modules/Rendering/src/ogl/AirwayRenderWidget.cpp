/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "AirwayRenderWidget.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <limits>
#include "Logger.h"

#include "rendering.configure.h"

AirwayRenderWidget::AirwayRenderWidget(const std::string& filePath, QWidget *parent /*= 0*/):
	textureID(0),
	shaderProgram(0),
	airwayTreeGeometry(0),
	shiftPressed(0),
	wPressed(0),
	aPressed(0),
	sPressed(0),
	dPressed(0),
	qPressed(0),
	ePressed(0),
	mouseLeftClick(0),
	mouseRightClick(0),
	QOpenGLWidget(parent)
{
	// Set focus such that keypresses and mouse clicks are registered in the main window
	setFocusPolicy(Qt::StrongFocus);

	// Set the filepath of the geometry
	// TODO :: Validity check
	airwayTreeFilepath = filePath;

	qDebug() << "AirwayRenderWidget Constructor - Done";
}

AirwayRenderWidget::~AirwayRenderWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    //delete texture;
	delete airwayTreeGeometry;
	doneCurrent();
}

void AirwayRenderWidget::initializeGL()
{
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (gladLoadGL())
		qDebug() << "GLAD Loaded";
	else
		qDebug() << "Error Loading GLAD";

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	
	//! [2]
	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Enable back face culling
	//glEnable(GL_CULL_FACE);
	//! [2]
	
	initAirwayShaders();
	airwayTreeGeometry = new AirwayTree(airwayTreeFilepath);
	
	camera.SetModelOffset(glm::vec3(2.0f, 55.0f, -150.0f)); // Around top of trachea
	camera.UpRotation(-90.0f);  // rotation to match OpenGL Coordinate system
	camera.SetKeyboardMovementSpeed(1.0f);
	camera.SetMouseMovementSpeed(0.035f);
	camera.SetMouseSensitivity(0.04f);

	/* LIGHTING for AIRWAY */
	lightPos = camera.GetPosition();
	lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
	lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	lightConstant = 1.0f;
	lightLinear = 0.00086f;
	lightQuadratic = 0.00004f;
	/* AirWay Specs */
	airwayExteriorColor = glm::vec3(1.0f, 0.5f, 1.0f);
	airwayInteriorColor = glm::vec3(0.0f, 1.0f, 1.0f);
	airwayShininess = 32.0f;
}

void AirwayRenderWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
	float aspect = float(w) / float(h ? h : 1);

	// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
	//float zFarInfinity = std::numeric_limits<float>::infinity(); // Nothing is rendered
	const float zNear = 0.1f, zFar = 1000000000.0f, fov = 45.0;

	// Reset projection
	projection = glm::mat4(1.0f);

	// Set perspective projection - no scroll callback so zoom doesnt change yet
	projection = glm::perspective(glm::radians(camera.GetZoom()), aspect, zNear, zFar);
}

void AirwayRenderWidget::paintGL()
{
	// Calculate model - view transformation
	// Camera remains fixed at (0,0,0), models are moved
	glm::vec3 modelOffset = camera.GetModelOffset();
	glm::vec3 geometryLocMean = airwayTreeGeometry->getGeometryMeanLocation();

	// Model Matrix 
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-geometryLocMean - modelOffset));

	// View matrix
	glm::mat4 view = camera.GetViewMatrix();

	glm::vec3 front = camera.GetFront();
	camera.GetCameraParameters(cameraParameters);
	
	qDebug() << "Front Direction: " << front.x << " " << front.y << " " << front.z;
	qDebug() << "Model Offset: " << modelOffset.x << " " << modelOffset.y << " " << modelOffset.z;
	qDebug() << "Roll: " << cameraParameters[ROLLCOORD] << " Pitch: " << cameraParameters[PITCHCOORD] << " Yaw: " << cameraParameters[YAWCOORD];

	mmOpenGLClearErrorMacro();

	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glBindTexture(GL_TEXTURE_2D, textureID);
	glUseProgram(shaderProgram);

	/* Passing information to Shader for Rendering */
	// pass MVP for vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	// pass lighting specifications into the shader
	glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "light.ambient"), 1, &lightAmbient[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "light.diffuse"), 1, &lightDiffuse[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "light.specular"), 1, &lightSpecular[0]);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.constant"), lightConstant);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.linear"), lightLinear);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.quadratic"), lightQuadratic);
	// pass cube information into the shader
	glUniform3fv(glGetUniformLocation(shaderProgram, "object.frontcolor"), 1, &airwayExteriorColor[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "object.backcolor"), 1, &airwayInteriorColor[0]);
	glUniform1f(glGetUniformLocation(shaderProgram, "object.shininess"), airwayShininess);

	mmOpenGLCheckErrorMacro("Error with using Shader for Painting Airway - OpenGL");

	airwayTreeGeometry->drawAirwayTreeGeometry();
}

void AirwayRenderWidget::initAirwayShaders()
{
	mmOpenGLClearErrorMacro();

	// Compile Shader program
	unsigned int vertex, fragment;
	vertex = compileShader(GL_VERTEX_SHADER, RENDERING_RESOURCES_DIR "/ogl/airwayVertexShader.glsl");
	checkCompileErrors(vertex, "VERTEX");
	fragment = compileShader(GL_FRAGMENT_SHADER, RENDERING_RESOURCES_DIR "/ogl/airwayFragmentShader.glsl");
	checkCompileErrors(fragment, "FRAGMENT");
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertex);
	glAttachShader(shaderProgram, fragment);
	glLinkProgram(shaderProgram);
	checkCompileErrors(shaderProgram, "PROGRAM");
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	glUseProgram(shaderProgram);

	mmOpenGLCheckErrorMacro("Error with initializing Shaders - OpenGL");
}

// void AirwayRenderWidget::initTextures()
// {
// 	mmOpenGLClearErrorMacro();
// 
// 	// My Code - Base OpenGL
// 	unsigned int textureNum;
// 	glGenTextures(1, &textureNum);
// 
// 	int width, height, nrComponents;
// 	std::string texturePath(DATA_DIR "/cube.png");
// 	const char *path = texturePath.c_str();
// 	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
// 	if (data)
// 	{
// 		GLenum format;
// 		if (nrComponents == 1)
// 			format = GL_RED;
// 		else if (nrComponents == 3)
// 			format = GL_RGB;
// 		else if (nrComponents == 4)
// 			format = GL_RGBA;
// 
// 		glBindTexture(GL_TEXTURE_2D, textureNum);
// 		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
// 		glGenerateMipmap(GL_TEXTURE_2D);
// 
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 
// 		stbi_image_free(data);
// 	}
// 	else
// 	{
// 		std::cout << "Texture failed to load at path: " << path << std::endl;
// 		stbi_image_free(data);
// 	}
// 
// 	textureID = textureNum;
// 	std::cout << "Texture loaded" << std::endl;
// 
// 	mmOpenGLCheckErrorMacro("Error with initializing Textures - OpenGL");
// }

// Compile either fragment or vertex shader
unsigned int AirwayRenderWidget::compileShader(unsigned int type, const char* shaderPath)
{
	const char* shaderType;
	if (type == GL_VERTEX_SHADER)
		shaderType = "Vertex";
	else if (type == GL_FRAGMENT_SHADER)
		shaderType = "Fragment";
	else
		shaderType = "Unknown";

	std::string code;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		// read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		// close file handlers
		shaderFile.close();
		// convert stream into string
		code = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		qDebug() << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ::" << shaderType;
	}
	const char* shaderCode = code.c_str();
	unsigned int id = glCreateShader(type);
	glShaderSource(id, 1, &shaderCode, 0);
	glCompileShader(id);

	qDebug() << "COMPILED SHADER::" << shaderType;

	return id;
}

// utility function for checking shader compilation/linking errors
void AirwayRenderWidget::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			qDebug() << "ERROR::SHADER_COMPILATION_ERROR of type:" << type.c_str() << "\n" << infoLog << "\n ------------------------------------------------------ ";
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			qDebug() << "ERROR::PROGRAM_LINKING_ERROR of type: " << type.c_str() << "\n" << infoLog << "\n ------------------------------------------------------ ";
		}
	}
}

void AirwayRenderWidget::mousePressEvent(QMouseEvent *e)
{
	mouseLastPosition = QVector2D(e->pos());

	switch (e->button())
	{
	case Qt::LeftButton:
		mouseLeftClick = true;
		break;
	case Qt::RightButton:
		mouseRightClick = true;
		break;
	default:
		mouseLeftClick = false;
		mouseRightClick = false;
		break;
	}
}

void AirwayRenderWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (mouseLeftClick)
	{
		QVector2D mouseLoc = QVector2D(e->pos());

		float xoffset = mouseLoc.x() - mouseLastPosition.x();
		float yoffset = mouseLoc.y() - mouseLastPosition.y();

		camera.ProcessMouseTranslation(xoffset, yoffset, shiftPressed);

		mouseLastPosition = QVector2D(e->pos());

		update();
	}
	else if (mouseRightClick)
	{
		QVector2D mouseLoc = QVector2D(e->pos());

		float xoffset = mouseLoc.x() - mouseLastPosition.x();
		float yoffset = mouseLoc.y() - mouseLastPosition.y();

		camera.ProcessMouseRotation(xoffset, yoffset, shiftPressed);

		mouseLastPosition = QVector2D(e->pos());

		update();
	}
	else
	{

	}
}

void AirwayRenderWidget::mouseReleaseEvent(QMouseEvent *e)
{
	mouseLeftClick = false;
	mouseRightClick = false;
}

// TO DO: Allow multiple key presses so camera can move an aggregate of forward and right for example
void AirwayRenderWidget::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Escape:
		exit(0);
		break;
	case Qt::Key_Shift:
		shiftPressed = true;
		update();
		break;
	case Qt::Key_W:
		wPressed = true;
		camera.ProcessKeyboardTranslation(FORWARD);
		update();
		break;
	case Qt::Key_A:
		aPressed = true;
		camera.ProcessKeyboardTranslation(LEFT);
		update();
		break;
	case Qt::Key_S:
		sPressed = true;
		camera.ProcessKeyboardTranslation(BACKWARD);
		update();
		break;
	case Qt::Key_D:
		dPressed = true;
		camera.ProcessKeyboardTranslation(RIGHT);
		update();
		break;
	case Qt::Key_Q:
		qPressed = true;
		camera.ProcessKeyboardTranslation(DOWN);
		update();
		break;
	case Qt::Key_E:
		ePressed = true;
		camera.ProcessKeyboardTranslation(UP);
		update();
		break;
	case Qt::Key_P:
		pPressed = true;
		camera.RightRotation(10);
		update();
		break;
	case Qt::Key_T:
		// reset camera top of trachea
		camera.SetDefault();
		camera.UpRotation(-90.0f);
		camera.SetModelOffset(glm::vec3(2.0f, 55.0f, -150.0f)); // Around top of trachea
		camera.SetKeyboardMovementSpeed(1.0f);
		camera.SetMouseMovementSpeed(0.035f);
		camera.SetMouseSensitivity(0.04f);
		update();
		break;
	case Qt::Key_Y:
		// reset camera to default (default constructor) (position 0,0,3 and look at origin)
		camera.SetDefault();
		update();
		break;
	default:
		shiftPressed = false;
		break;
	}

	e->accept();  // Don't pass any key events to parent
}

void AirwayRenderWidget::keyReleaseEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	default:
		break;
	}
}