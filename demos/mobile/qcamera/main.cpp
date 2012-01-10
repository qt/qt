/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "cameraexample.h"

#include <QtGui>
#include <QApplication>
#include <QDebug>

// Lock Symbian orientation
#ifdef Q_OS_SYMBIAN
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif


#ifdef Q_OS_SYMBIAN
#include <QSymbianEvent>
#include <w32std.h>

#endif

static const int KGoomMemoryLowEvent = 0x10282DBF;
static const int KGoomMemoryGoodEvent = 0x20026790;

class MyApplication : public QApplication
{
public:
  MyApplication( int argc, char** argv ) : QApplication( argc, argv ) {}

#ifdef Q_OS_SYMBIAN
protected:
//! [0]
  bool symbianEventFilter( const QSymbianEvent* symbianEvent )
  {
    const TWsEvent *event = symbianEvent->windowServerEvent();
    if ( !event ) {
      return false;
    }
    switch ( event->Type() ) {
      // GOOM handling enabled
      // http://wiki.forum.nokia.com/index.php/Graphics_memory_handling
      case EEventUser: {
        TApaSystemEvent* eventData = reinterpret_cast<TApaSystemEvent*>(event->EventData());
        if ((*eventData) == EApaSystemEventShutdown) {
            eventData++;
            if ((*eventData) == KGoomMemoryLowEvent) {
             return true;
            }
        }
        break;
      }
      default:
        break;
    };

    // Always return false so we don't stop
    // the event from being processed
    return false;
  }
//! [0]
#endif
};


int main(int argc, char *argv[])
{
    // NOTE: set this value before creating MyApplication instance
    // http://doc.trolltech.com/qapplication.html#setGraphicsSystem
    QApplication::setGraphicsSystem("raster"); // NOTE: Seems that raster have to be enabled with Nokia N8

    MyApplication a(argc, argv);

    // Lock Symbian orientation
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE(
    if (appUi) {
        appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
    }
    );
#endif

    CameraExample w;
    w.showFullScreen();

    return a.exec();
}
