#ifndef QXPIXMAPREADBACKGLCONTEXT_H
#define QXPIXMAPREADBACKGLCONTEXT_H

#include <QPlatformGLContext>
#include <QtGui/QWidget>

#include "qwaylandxpixmapeglintegration.h"
#include "qwaylandxpixmapwindow.h"

class QWaylandShmBuffer;

class QXPixmapReadbackGLContext : public QPlatformGLContext
{
public:
    QXPixmapReadbackGLContext(QWaylandXPixmapEglIntegration *eglIntegration, QWaylandXPixmapWindow *window);
    ~QXPixmapReadbackGLContext();

    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void* getProcAddress(const QString& procName);

    virtual QPlatformWindowFormat platformWindowFormat() const;

    void geometryChanged();

private:
    QWaylandXPixmapEglIntegration *mEglIntegration;
    QWaylandXPixmapWindow *mWindow;
    QWaylandShmBuffer *mBuffer;

    Pixmap mPixmap;

    EGLConfig mConfig;
    EGLContext mContext;
    EGLSurface mPixmapSurface;
};

#endif // QXPIXMAPREADBACKGLCONTEXT_H
