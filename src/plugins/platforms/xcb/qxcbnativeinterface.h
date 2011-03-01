#ifndef QXCBNATIVEINTERFACE_H
#define QXCBNATIVEINTERFACE_H

#include <QtGui/QPlatformNativeInterface>

class QWidget;
class QXcbScreen;

class QXcbNativeInterface : public QPlatformNativeInterface
{
public:
    enum ResourceType {
        Display,
        EglDisplay,
        Connection,
        Screen,
        GraphicsDevice
    };

    void *nativeResourceForWidget(const QByteArray &resourceString, QWidget *widget);

    void *displayForWidget(QWidget *widget);
    void *eglDisplayForWidget(QWidget *widget);
    void *connectionForWidget(QWidget *widget);
    void *screenForWidget(QWidget *widget);
    void *graphicsDeviceForWidget(QWidget *widget);

private:
    static QXcbScreen *qPlatformScreenForWidget(QWidget *widget);
};

#endif // QXCBNATIVEINTERFACE_H
