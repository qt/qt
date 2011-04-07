#ifndef QWAYLANDXCOMPOSITEEGLCONTEXT_H
#define QWAYLANDXCOMPOSITEEGLCONTEXT_H

#include <QtGui/QPlatformGLContext>

#include <QtCore/QWaitCondition>

#include "qwaylandbuffer.h"
#include "qwaylandxcompositeeglintegration.h"

class QWaylandXCompositeEGLWindow;

class QWaylandXCompositeEGLContext : public QPlatformGLContext
{
public:
    QWaylandXCompositeEGLContext(QWaylandXCompositeEGLIntegration *glxIntegration, QWaylandXCompositeEGLWindow *window);

    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void* getProcAddress(const QString& procName);

    QPlatformWindowFormat platformWindowFormat() const;

    void geometryChanged();

private:
    QWaylandXCompositeEGLIntegration *mEglIntegration;
    QWaylandXCompositeEGLWindow *mWindow;
    QWaylandBuffer *mBuffer;

    Window mXWindow;
    EGLConfig mConfig;
    EGLContext mContext;
    EGLSurface mEglWindowSurface;

    static void sync_function(void *data);
    bool mWaitingForSync;
};

#endif // QWAYLANDXCOMPOSITEEGLCONTEXT_H
