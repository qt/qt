/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    QDeclarativeView view;

#ifndef SHADEREFFECTS_USE_OPENGL_GRAPHICSSYSTEM
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);
    format.setSwapInterval(1);
    QGLWidget* glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    view.setViewport(glWidget);
#endif

    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view.setAttribute(Qt::WA_OpaquePaintEvent);
    view.setAttribute(Qt::WA_NoSystemBackground);
    view.setSource(QUrl::fromLocalFile(QLatin1String("qml/main.qml")));
    QObject::connect(view.engine(), SIGNAL(quit()), &view, SLOT(close()));

    view.show();

    return app.exec();
}
