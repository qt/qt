/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtDeclarative>
#include <QtGui>
#include <QAccelerometer>

#include "accelerometerfilter.h"
#include "settings.h"
#include "taskswitcher.h"

// Lock orientation in Symbian
#ifdef Q_OS_SYMBIAN
    #include <eikenv.h>
    #include <eikappui.h>
    #include <aknenv.h>
    #include <aknappui.h>
#endif

QTM_USE_NAMESPACE


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#ifdef Q_OS_SYMBIAN
    // Lock orientation to landscape in Symbian
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE(
        if (appUi)
            appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
    )
#endif

    //! [0]
    QDeclarativeView view;
    view.setSource(QUrl("qrc:/qml/BubbleLevel.qml"));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    //! [0]

    // ![1]
    Settings settings;

    QAccelerometer sensor;
    AccelerometerFilter filter;
    sensor.addFilter(&filter);
    //! [1]

    //! [2]
    QObject *rootObject = dynamic_cast<QObject*>(view.rootObject());

    // Associate Qt / QML signals and slots
    QObject::connect(rootObject, SIGNAL(saveCorrectionAngle(const QVariant&)),
                     &settings, SLOT(saveCorrectionAngle(const QVariant&)));

    QObject::connect(&filter, SIGNAL(rotationChanged(const QVariant&)),
                     rootObject, SLOT(handleRotation(const QVariant&)));

    QObject::connect(&settings, SIGNAL(correctionAngle(const QVariant&)),
                     rootObject, SLOT(setCorrectionAngle(const QVariant&)));

    QObject::connect((QObject*)view.engine(), SIGNAL(quit()),
                     &app, SLOT(quit()));
    //! [2]

    //! [3]
#ifdef Q_WS_MAEMO_5
    TaskSwitcher taskSwitcher;

    QObject::connect(rootObject, SIGNAL(minimizeApplication()),
                     &taskSwitcher, SLOT(minimizeApplication()));

    // Show the task switcher button
    rootObject->setProperty("taskSwitcherVisible", true);
#endif
    //! [3]

    //! [4]
    // Read correction factor from permanent storage and emit it to QML side
    settings.loadAndEmitCorrectionAngle();

    // Begin measuring of the accelerometer sensor
    sensor.start();
    //! [4]

    //! [5]
#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    view.setGeometry(QApplication::desktop()->screenGeometry());
    view.showFullScreen();
#else
    view.setGeometry((QRect(100, 100, 800, 480)));
    view.show();
#endif
    //! [5]

    return app.exec();
}
