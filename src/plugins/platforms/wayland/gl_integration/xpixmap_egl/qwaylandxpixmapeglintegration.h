#ifndef QWAYLANDXPIXMAPEGLINTEGRATION_H
#define QWAYLANDXPIXMAPEGLINTEGRATION_H

#include "gl_integration/qwaylandglintegration.h"

#include <QtCore/QTextStream>
#include <QtCore/QDataStream>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>
#include <QtGui/QWidget>

#include <X11/Xlib.h>

#include <EGL/egl.h>
//#include <EGL

class QWaylandXPixmapEglIntegration : public QWaylandGLIntegration
{
public:
    QWaylandXPixmapEglIntegration(QWaylandDisplay *display);
    ~QWaylandXPixmapEglIntegration();

    void initialize();
    QWaylandWindow *createEglWindow(QWidget *widget);

    QWaylandDisplay *waylandDisplay() const;
    Display *xDisplay() const;
    Window rootWindow() const;
    int depth() const;

    EGLDisplay eglDisplay();

private:
    QWaylandDisplay *mWaylandDisplay;
    Display *mDisplay;
    int mScreen;
    Window mRootWindow;
    EGLDisplay mEglDisplay;

};

#endif // QWAYLANDXPIXMAPEGLINTEGRATION_H
