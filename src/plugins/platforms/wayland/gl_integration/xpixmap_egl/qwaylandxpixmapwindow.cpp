#include "qwaylandxpixmapwindow.h"

#include "qwaylandxpixmapeglcontext.h"
QWaylandXPixmapWindow::QWaylandXPixmapWindow(QWidget *window, QWaylandXPixmapEglIntegration *eglIntegration)
    : QWaylandShmWindow(window)
    , mEglIntegration(eglIntegration)
    , mContext(0)
{
}

QWaylandWindow::WindowType QWaylandXPixmapWindow::windowType() const
{
    //We'r lying, maybe we should add a type, but for now it will do
    //since this is primarly used by the windowsurface.
    return QWaylandWindow::Egl;
}

QPlatformGLContext *QWaylandXPixmapWindow::glContext() const
{
    if (!mContext) {
        QWaylandXPixmapWindow *that = const_cast<QWaylandXPixmapWindow *>(this);
        that->mContext = new QXPixmapReadbackGLContext(mEglIntegration,that);
    }
    return mContext;
}

void QWaylandXPixmapWindow::setGeometry(const QRect &rect)
{
    QPlatformWindow::setGeometry(rect);

    if (mContext)
        mContext->geometryChanged();
}

