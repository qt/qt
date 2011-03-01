#include "qxcbnativeinterface.h"

#include "qxcbscreen.h"

#include <QtGui/private/qapplication_p.h>

QXcbScreen *QXcbNativeInterface::screenForWidget(QWidget *widget)
{
    QXcbScreen *screen;
    if (widget) {
        screen = static_cast<QXcbScreen *>(QPlatformScreen::platformScreenForWidget(widget));
    }else {
        screen = static_cast<QXcbScreen *>(QApplicationPrivate::platformIntegration()->screens()[0]);
    }
    return screen;
}

void * QXcbNativeInterface::nativeDisplayForWidget(QWidget *widget)
{
#if defined(XCB_USE_XLIB)
    QXcbScreen *screen = screenForWidget(widget);
    return screen->connection()->xlib_display();
#else
    Q_UNUSED(widget);
    return 0;
#endif
}

void * QXcbNativeInterface::eglDisplayForWidget(QWidget *widget)
{
#if defined(XCB_USE_DRI2)
    QXcbScreen *screen = screenForWidget(widget);
    return screen->connection()->egl_display();
#else
    Q_UNUSED(widget)
    return 0;
#endif
}

void * QXcbNativeInterface::nativeConnectionForWidget(QWidget *widget)
{
    QXcbScreen *screen = screenForWidget(widget);
    return screen->xcb_connection();
}

void * QXcbNativeInterface::nativeScreenForWidget(QWidget *widget)
{
    QXcbScreen *screen = screenForWidget(widget);
    return screen->screen();
}
