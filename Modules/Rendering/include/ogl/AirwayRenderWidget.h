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

#ifndef AIRWAYRENDERWIDGET_H
#define AIRWAYRENDERWIDGET_H

// #include "MainWindow.h" // TODO :: Remove this file
#include "AirwayTreeEngine.h"
#include "Camera6DoF.h"

#include <QOpenGLWidget>
#include <QVector2D>
#include <QMouseEvent>
#include <QKeyEvent>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

class AirwayRenderWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    //explicit MainWidget(QWidget *parent = 0);
	AirwayRenderWidget(QWidget *parent = 0);
    ~AirwayRenderWidget();

protected:
    
	void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

	void initAirwayShaders();

	/* supporting functions */
	unsigned int compileShader(unsigned int type, const char* shaderPath);
	void checkCompileErrors(unsigned int shader, std::string type);

	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void keyPressEvent(QKeyEvent *e) override;
	void keyReleaseEvent(QKeyEvent *e) override;
	
private:	
	/* Shader */
	unsigned int shaderProgram; //Standard OpenGL
    
	/* Geometry */
	AirwayTree *airwayTreeGeometry;

	/* Texture */
    unsigned int textureID;

	/* MVP Matrices */
    glm::mat4 projection;

	/* Lighting */
	glm::vec3 lightPos;
	glm::vec3 lightAmbient;
	glm::vec3 lightDiffuse;
	glm::vec3 lightSpecular;
	float lightConstant = 1.0f;
	float lightLinear = 0.09f;
	float lightQuadratic = 0.032f;
	glm::vec3 cubeColor;
	glm::vec3 airwayInteriorColor;
	glm::vec3 airwayExteriorColor;
	float airwayShininess;

	/* Camera */
	Camera6DoF camera;

	/* Mouse Events */
	glm::vec2 mousePressPosition;
	glm::vec2 mouseReleasePosition;
	QVector2D mouseLastPosition;
	
	/* Misc */
	//MainWindow *m_mainWindow;
	bool shiftPressed;
	bool wPressed;
	bool aPressed;
	bool sPressed;
	bool dPressed;
	bool qPressed;
	bool ePressed;
	bool pPressed;
	bool mouseLeftClick;
	bool mouseRightClick;	
	float cameraParameters[6];
};

#endif // AIRWAYRENDERWIDGET_H