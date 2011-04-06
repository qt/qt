#ifndef QWAYLANDXCOMPOSITEGLXCONTEXT_H
#define QWAYLANDXCOMPOSITEGLXCONTEXT_H

#include <QtGui/QPlatformGLContext>

#include <QtCore/QWaitCondition>

#include "qwaylandbuffer.h"
#include "qwaylandxcompositeglxintegration.h"

#include "qglxconvenience.h"

class QWaylandXCompositeGLXWindow;
class QWaylandShmBuffer;

class QWaylandXCompositeGLXContext : public QPlatformGLContext
{
public:
    QWaylandXCompositeGLXContext(QWaylandXCompositeGLXIntegration *glxIntegration, QWaylandXCompositeGLXWindow *window);

    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void* getProcAddress(const QString& procName);

    QPlatformWindowFormat platformWindowFormat() const;

    void geometryChanged();

private:
    QWaylandXCompositeGLXIntegration *mGlxIntegration;
    QWaylandXCompositeGLXWindow *mWindow;
    QWaylandBuffer *mBuffer;

    Window mXWindow;
    GLXFBConfig mConfig;
    GLXContext mContext;

    static void sync_function(void *data);
    QWaitCondition mWaitCondition;
};

#endif // QWAYLANDXCOMPOSITEGLXCONTEXT_H
