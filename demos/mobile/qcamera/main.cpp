/**
 * Copyright (c) 2011 Nokia Corporation. All rights reserved.
 *
 * Nokia and Nokia Connecting People are registered trademarks of Nokia
 * Corporation. Oracle and Java are registered trademarks of Oracle and/or its
 * affiliates. Other product and company names mentioned herein may be
 * trademarks or trade names of their respective owners.
 *
 *
 * Subject to the conditions below, you may, without charge:
 *
 *  *  Use, copy, modify and/or merge copies of this software and associated
 *     documentation files (the "Software")
 *
 *  *  Publish, distribute, sub-licence and/or sell new software derived from
 *     or incorporating the Software.
 *
 *
 *
 * This file, unmodified, shall be included with all copies or substantial
 * portions of the Software that are distributed in source code form.
 *
 * The Software cannot constitute the primary value of any new software derived
 * from or incorporating the Software.
 *
 * Any person dealing with the Software shall not misrepresent the source of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
