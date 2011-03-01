#include "qxcbnativeinterface.h"

#include "qxcbscreen.h"

#include <QtGui/private/qapplication_p.h>
#include <QtCore/QMap>

class QXcbResourceMap : public QMap<QByteArray, QXcbNativeInterface::ResourceType>
{
public:
    QXcbResourceMap()
        :QMap<QByteArray, QXcbNativeInterface::ResourceType>()
    {
        insert("display",QXcbNativeInterface::Display);
        insert("egldisplay",QXcbNativeInterface::EglDisplay);
        insert("connection",QXcbNativeInterface::Connection);
        insert("screen",QXcbNativeInterface::Screen);
        insert("graphicsdevice",QXcbNativeInterface::GraphicsDevice);
    }
};

Q_GLOBAL_STATIC(QXcbResourceMap, qXcbResourceMap)

void *QXcbNativeInterface::nativeResourceForWidget(const QByteArray &resourceString, QWidget *widget)
{
    QByteArray lowerCaseResource = resourceString.toLower();
    ResourceType resource = qXcbResourceMap()->value(lowerCaseResource);
    void *result = 0;
    switch(resource) {
    case Display:
        result = displayForWidget(widget);
        break;
    case EglDisplay:
        result = eglDisplayForWidget(widget);
        break;
    case Connection:
        result = connectionForWidget(widget);
        break;
    case Screen:
        result = qPlatformScreenForWidget(widget);
        break;
    case GraphicsDevice:
        result = graphicsDeviceForWidget(widget);
        break;
    default:
        result = 0;
    }
    return result;
}

QXcbScreen *QXcbNativeInterface::qPlatformScreenForWidget(QWidget *widget)
{
    QXcbScreen *screen;
    if (widget) {
        screen = static_cast<QXcbScreen *>(QPlatformScreen::platformScreenForWidget(widget));
    }else {
        screen = static_cast<QXcbScreen *>(QApplicationPrivate::platformIntegration()->screens()[0]);
    }
    return screen;
}

void *QXcbNativeInterface::displayForWidget(QWidget *widget)
{
#if defined(XCB_USE_XLIB)
    QXcbScreen *screen = qPlatformScreenForWidget(widget);
    return screen->connection()->xlib_display();
#else
    Q_UNUSED(widget);
    return 0;
#endif
}

void *QXcbNativeInterface::eglDisplayForWidget(QWidget *widget)
{
#if defined(XCB_USE_DRI2)
    QXcbScreen *screen = qPlatformScreenForWidget(widget);
    return screen->connection()->egl_display();
#else
    Q_UNUSED(widget)
    return 0;
#endif
}

void *QXcbNativeInterface::connectionForWidget(QWidget *widget)
{
    QXcbScreen *screen = qPlatformScreenForWidget(widget);
    return screen->xcb_connection();
}

void *QXcbNativeInterface::screenForWidget(QWidget *widget)
{
    QXcbScreen *screen = qPlatformScreenForWidget(widget);
    return screen->screen();
}

void *QXcbNativeInterface::graphicsDeviceForWidget(QWidget *widget)
{
#if defined(XCB_USE_DRI2)
    QXcbScreen *screen = qPlatformScreenForWidget(widget);
    QByteArray deviceName = screen->connection()->dri2DeviceName();
    return deviceName.data();
#else
    return 0;
#endif

}
