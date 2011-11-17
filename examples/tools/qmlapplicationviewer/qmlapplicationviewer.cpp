/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

/*
  This file was created by the Qt Quick Application wizard of Qt Creator.
  QmlApplicationViewer is a convenience class containing mobile device specific
  code such as screen orientation handling. Also QML paths and debugging are
  handled here.
  It is recommended not to modify this file, since newer versions of Qt Creator
  may offer an updated version of it.
*/

#include "qmlapplicationviewer.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>

#if defined(QMLJSDEBUGGER)
#include <qt_private/qdeclarativedebughelper_p.h>
#endif

#if defined(QMLJSDEBUGGER) && !defined(NO_JSDEBUGGER)
#include <jsdebuggeragent.h>
#endif
#if defined(QMLJSDEBUGGER) && !defined(NO_QMLOBSERVER)
#include <qdeclarativeviewobserver.h>
#endif

#if defined(Q_OS_SYMBIAN) && defined(ORIENTATIONLOCK)
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif // Q_OS_SYMBIAN && ORIENTATIONLOCK

#if defined(QMLJSDEBUGGER)

// Enable debugging before any QDeclarativeEngine is created
struct QmlJsDebuggingEnabler
{
    QmlJsDebuggingEnabler()
    {
        QDeclarativeDebugHelper::enableDebugging();
    }
};

// Execute code in constructor before first QDeclarativeEngine is instantiated
static QmlJsDebuggingEnabler enableDebuggingHelper;

#endif // QMLJSDEBUGGER

class QmlApplicationViewerPrivate
{
    QString mainQmlFile;
    friend class QmlApplicationViewer;
    static QString adjustPath(const QString &path);
};

QString QmlApplicationViewerPrivate::adjustPath(const QString &path)
{
#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
    if (!QDir::isAbsolutePath(path))
        return QCoreApplication::applicationDirPath()
                + QLatin1String("/../Resources/") + path;
#else
    const QString pathInShareDir = QCoreApplication::applicationDirPath()
        + QLatin1String("/../share/")
        + QFileInfo(QCoreApplication::applicationFilePath()).fileName()
        + QLatin1Char('/') + path;
    if (QFileInfo(pathInShareDir).exists())
        return pathInShareDir;
#endif
#endif
    return path;
}

QmlApplicationViewer::QmlApplicationViewer(QWidget *parent) :
    QDeclarativeView(parent),
    m_d(new QmlApplicationViewerPrivate)
{
    connect(engine(), SIGNAL(quit()), SLOT(close()));
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
#if defined(QMLJSDEBUGGER) && !defined(NO_JSDEBUGGER)
    new QmlJSDebugger::JSDebuggerAgent(engine());
#endif
#if defined(QMLJSDEBUGGER) && !defined(NO_QMLOBSERVER)
    new QmlJSDebugger::QDeclarativeViewObserver(this, parent);
#endif
}

QmlApplicationViewer::~QmlApplicationViewer()
{
    delete m_d;
}

void QmlApplicationViewer::setMainQmlFile(const QString &file)
{
    m_d->mainQmlFile = QmlApplicationViewerPrivate::adjustPath(file);
    setSource(QUrl::fromLocalFile(m_d->mainQmlFile));
}

void QmlApplicationViewer::addImportPath(const QString &path)
{
    engine()->addImportPath(QmlApplicationViewerPrivate::adjustPath(path));
}

void QmlApplicationViewer::setOrientation(ScreenOrientation orientation)
{
#ifdef Q_OS_SYMBIAN
    if (orientation != ScreenOrientationAuto) {
#if defined(ORIENTATIONLOCK)
        const CAknAppUiBase::TAppUiOrientation uiOrientation =
                (orientation == ScreenOrientationLockPortrait) ? CAknAppUi::EAppUiOrientationPortrait
                    : CAknAppUi::EAppUiOrientationLandscape;
        CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
        TRAPD(error,
            if (appUi)
                appUi->SetOrientationL(uiOrientation);
        );
        Q_UNUSED(error)
#else // ORIENTATIONLOCK
        qWarning("'ORIENTATIONLOCK' needs to be defined on Symbian when locking the orientation.");
#endif // ORIENTATIONLOCK
    }
#elif defined(Q_WS_MAEMO_5)
    Qt::WidgetAttribute attribute;
    switch (orientation) {
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_Maemo5PortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_Maemo5LandscapeOrientation;
        break;
    case ScreenOrientationAuto:
    default:
        attribute = Qt::WA_Maemo5AutoOrientation;
        break;
    }
    setAttribute(attribute, true);
#else // Q_OS_SYMBIAN
    Q_UNUSED(orientation);
#endif // Q_OS_SYMBIAN
}

void QmlApplicationViewer::showExpanded()
{
#ifdef Q_OS_SYMBIAN
    showFullScreen();
#elif defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    showMaximized();
#else
    show();
#endif
}
