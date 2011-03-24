#ifndef QWAYLANDXPIXMAPGLXCONTEXT_H
#define QWAYLANDXPIXMAPGLXCONTEXT_H

#include <QPlatformGLContext>

#include "qwaylandxpixmapglxintegration.h"

#include "qglxconvenience.h"

class QWaylandXPixmapGLXWindow;
class QWaylandShmBuffer;

class QWaylandXPixmapGLXContext : public QPlatformGLContext
{
public:
    QWaylandXPixmapGLXContext(QWaylandXPixmapGLXIntegration *glxIntegration, QWaylandXPixmapGLXWindow *window);

    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void* getProcAddress(const QString& procName);

    QPlatformWindowFormat platformWindowFormat() const;

    void geometryChanged();

private:
    QWaylandXPixmapGLXIntegration *mGlxIntegration;
    QWaylandXPixmapGLXWindow *mWindow;
    QWaylandShmBuffer *mBuffer;

    Pixmap mPixmap;
    GLXFBConfig mConfig;
    GLXContext mContext;
    GLXPixmap mGlxPixmap;
};

#endif // QWAYLANDXPIXMAPGLXCONTEXT_H
