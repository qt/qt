/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
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

#include <QtGui/QApplication>
#include <QDesktopWidget>
#include "mainwindow.h"

// Lock S60 orientation
#ifdef Q_OS_SYMBIAN
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif

#ifndef QT_NO_OPENGL
    #include <QGLWidget>
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Lock S60 orientation
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAPD(error,
    if (appUi) {
        appUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait);
    }
    );
#endif

    MainWindow w;

#ifndef QT_NO_OPENGL
    // Use QGLWidget to get the opengl support if available
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);

    QGLWidget *glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    w.setViewport(glWidget);     // ownership of glWidget is taken
#endif

    QObject *rootObject = dynamic_cast<QObject*>(w.rootObject());

    // For Qt.exit() from QML
    QObject::connect((QObject*)w.engine(), SIGNAL(quit()), &a, SLOT(quit()));
    // Level activation signal from QML
    QObject::connect(rootObject, SIGNAL(levelActivated(int)), &w, SLOT(levelActivated(int)));

#if defined (Q_OS_SYMBIAN) || defined (Q_WS_MAEMO_5)
    w.setGeometry(QApplication::desktop()->screenGeometry());
    w.showFullScreen();
#else
    w.setGeometry(100,100,360,640);
    w.setFixedSize(360,640);
    w.show();
#endif

    return a.exec();
}
