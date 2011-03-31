#include "qwaylandxpixmapglxwindow.h"

QWaylandXPixmapGLXWindow::QWaylandXPixmapGLXWindow(QWidget *window, QWaylandXPixmapGLXIntegration *glxIntegration)
    : QWaylandShmWindow(window)
    , mGlxIntegration(glxIntegration)
    , mContext(0)
{
}

QWaylandWindow::WindowType QWaylandXPixmapGLXWindow::windowType() const
{
    //yeah. this type needs a new name
    return QWaylandWindow::Egl;
}

QPlatformGLContext * QWaylandXPixmapGLXWindow::glContext() const
{
    if (!mContext) {
        QWaylandXPixmapGLXWindow *that = const_cast<QWaylandXPixmapGLXWindow *>(this);
        that->mContext = new QWaylandXPixmapGLXContext(mGlxIntegration,that);
    }
    return mContext;
}

void QWaylandXPixmapGLXWindow::setGeometry(const QRect &rect)
{
    QWaylandShmWindow::setGeometry(rect);

    if (mContext) {
        mContext->geometryChanged();
    }
}
