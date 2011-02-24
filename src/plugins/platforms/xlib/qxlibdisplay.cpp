#include "qxlibdisplay.h"

QXlibDisplay::QXlibDisplay(Display *display)
    : mDisplay(display)
{
    if (!mDisplay) {
        qFatal("Cannot connect to X server");
    }
    mDisplayName = QString::fromLocal8Bit(DisplayString(mDisplay));
}

QXlibDisplay::~QXlibDisplay()
{
    XCloseDisplay(mDisplay);
}

QString QXlibDisplay::displayName() const
{
     { return mDisplayName; }
}



Display * QXlibDisplay::nativeDisplay() const
{
    return mDisplay;
}

void QXlibDisplay::sync() const
{
    XSync(mDisplay, False);
}

void QXlibDisplay::flush() const
{
    XFlush(mDisplay);
}
