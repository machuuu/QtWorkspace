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

#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>

#include "glad/glad.h"
#include "mmOpenGLErrorCheck.h"

#ifndef QT_NO_OPENGL
#include "AirwayRenderWidget.h"
#include "AxialSlicerWidget.h"
#include "CoronalSlicerWidget.h"
#include "SagittalSlicerWidget.h"
#include "AxialSlabWidget.h"
#include "MainWindow.h"
#include "SlicerDisplay.h"
#include "ObliqueSlicerWidget.h"
#endif

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	//std::cout << QCoreApplication::testAttribute(Qt::AA_ShareOpenGLContexts) << std::endl; \\ returned true
	QApplication app(argc, argv);

	//QSurfaceFormat format;
	//format.setDepthBufferSize(24);
	//QSurfaceFormat::setDefaultFormat(format);
	//app.setApplicationName("cube");
	//app.setApplicationVersion("0.1");	

	std::string headerFilename("X:/MIPL/Cases/20349 Cases/20349.3.90/20349.3.90/20349_3_90_B31f.hdr");
	std::string imageFilename("X:/MIPL/Cases/20349 Cases/20349.3.90/20349.3.90/20349_3_90_B31f.img");
	//std::string headerFilename("X:/MIPL/Cases/20349 Cases/20349.3.91/CT/20349_3_91_B31f.hdr");
	//std::string imageFilename("X:/MIPL/Cases/20349 Cases/20349.3.91/CT/20349_3_91_B31f.img");
	//std::string headerFilename("X:/MIPL/Cases/20349 Cases/20349_3_97/20349_3_97_B31f.hdr");
	//std::string imageFilename("X:/MIPL/Cases/20349 Cases/20349_3_97/20349_3_97_B31f.img");
	//std::string headerFilename("X:/MIPL/Cases/20349 Cases/20349_3_85/20349_3_85_B31f.hdr");
	//std::string imageFilename("X:/MIPL/Cases/20349 Cases/20349_3_85/20349_3_85_B31f.img");
	//std::string headerFilename("X:/MIPL/Cases/21405_3a_HiDefTesting/21405.3a.hdr");
	//std::string imageFilename("X:/MIPL/Cases/21405_3a_HiDefTesting/21405.3a.img");
	CTVolume *CTData = new CTVolume(headerFilename, imageFilename);
	
#ifndef QT_NO_OPENGL
	// Widgets
	//AirwayRenderWidget *airwayRender = new AirwayRenderWidget();
	//AxialSlicerWidget *axialSlicer = new AxialSlicerWidget(CTData);
	//CoronalSlicerWidget *coronalSlicer = new CoronalSlicerWidget(CTData);
	//SagittalSlicerWidget *sagittalSlicer = new SagittalSlicerWidget(CTData);
	//AxialSlabWidget *axialSlab = new AxialSlabWidget(CTData);

	//airwayRender->setWindowTitle("Inner Airway Tree");
	//axialSlicer->setWindowTitle("Axial Slicer");
	//coronalSlicer->setWindowTitle("Coronal Slicer");
	//sagittalSlicer->setWindowTitle("Sagittal Slicer");
	
	// Signals and slots
	//QObject::connect(axialSlicer, SIGNAL(axialSliceSelect(QVector2D)), sagittalSlicer, SLOT(axialChangeSlice(QVector2D)));
	//QObject::connect(axialSlicer, SIGNAL(axialSliceSelect(QVector2D)), coronalSlicer, SLOT(axialChangeSlice(QVector2D)));

	//QObject::connect(coronalSlicer, SIGNAL(coronalSliceSelect(QVector2D)), sagittalSlicer, SLOT(coronalChangeSlice(QVector2D)));
	//QObject::connect(coronalSlicer, SIGNAL(coronalSliceSelect(QVector2D)), axialSlicer, SLOT(coronalChangeSlice(QVector2D)));

	//QObject::connect(sagittalSlicer, SIGNAL(sagittalSliceSelect(QVector2D)), coronalSlicer, SLOT(sagittalChangeSlice(QVector2D)));
	//QObject::connect(sagittalSlicer, SIGNAL(sagittalSliceSelect(QVector2D)), axialSlicer, SLOT(sagittalChangeSlice(QVector2D)));

	// Display
	//airwayRender->show();
	
	// Displaying Single Widgets
	//axialSlicer->resize(300, 300 / CTData->getAxialAspectRatio());
	//axialSlicer->show();
	//coronalSlicer->resize(300, 300 / CTData->getCoronalAspectRatio());
	//coronalSlicer->show();
	//sagittalSlicer->resize(300, 300 / CTData->getSagittalAspectRatio());
	//sagittalSlicer->show();

	// Embed the slicer widgets into a main widget that handles resizing, Voxel Picking, etc..
	//SlicerDisplay *mainAxial = new SlicerDisplay(axialSlicer, 300, (300 / CTData->getAxialAspectRatio()));
	//mainAxial->show();
	//SlicerDisplay *mainCoronal = new SlicerDisplay(coronalSlicer, 300, (300 / CTData->getCoronalAspectRatio()));
	//mainCoronal->show();
	//SlicerDisplay *mainSagittal = new SlicerDisplay(sagittalSlicer, 300, (300 / CTData->getSagittalAspectRatio()));
	//mainSagittal->show();
	
	//axialSlab->resize(300, 300 / CTData->getAxialAspectRatio());
	//axialSlab->show();
	
	//SlicerDisplay *mainOblique = new SlicerDisplay(obliqueSlicer, 300, (300 / CTData->getAxialAspectRatio()));
	//mainOblique->show();

	ObliqueSlicerWidget *mainOblique = new ObliqueSlicerWidget(CTData);
	mainOblique->resize(512, 512);
	mainOblique->show();

	//mainAxial->setWindowTitle("Axial Slicer");
	//mainCoronal->setWindowTitle("Coronal Slicer");
	//mainSagittal->setWindowTitle("Sagittal Slicer");
	//mainOblique->setWindowTitle("Oblique Slicer");
	
	//MainWindow mw;
	//mw.setFixedSize(1500, 1000);
	//mw.show();
	//MainWidget widget;
	//widget.show();
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
    return app.exec();
}


