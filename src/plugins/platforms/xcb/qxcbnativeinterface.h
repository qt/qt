#ifndef QXCBNATIVEINTERFACE_H
#define QXCBNATIVEINTERFACE_H

#include <QtGui/QPlatformNativeInterface>

class QWidget;
class QXcbScreen;

class QXcbNativeInterface : public QPlatformNativeInterface
{
    void * nativeDisplayForWidget(QWidget *widget);
    void * eglDisplayForWidget(QWidget *widget);
    void * nativeConnectionForWidget(QWidget *widget);
    void * nativeScreenForWidget(QWidget *widget);

private:
    static QXcbScreen *screenForWidget(QWidget *widget);
};

#endif // QXCBNATIVEINTERFACE_H
