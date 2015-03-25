/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlapplicationviewer.h"
#include <QtGui/QApplication>
#include <QtOpenGL>
#include <QDeclarativeView>
#include <QDeclarativeEngine>

int main(int argc, char *argv[])
{
// Depending on which is the recommended way for the platform, either use
// opengl graphics system or paint into QGLWidget.
#ifdef SHADEREFFECTS_USE_OPENGL_GRAPHICSSYSTEM
    QApplication::setGraphicsSystem("opengl");
#endif

    QApplication app(argc, argv);
    QmlApplicationViewer viewer;

#ifndef SHADEREFFECTS_USE_OPENGL_GRAPHICSSYSTEM
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);
    format.setSwapInterval(1);
    QGLWidget* glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    viewer.setViewport(glWidget);
#endif

    viewer.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    viewer.setAttribute(Qt::WA_OpaquePaintEvent);
    viewer.setAttribute(Qt::WA_NoSystemBackground);
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/shadereffects/main.qml"));
    QObject::connect(viewer.engine(), SIGNAL(quit()), &viewer, SLOT(close()));
    viewer.showExpanded();

    return app.exec();
}
