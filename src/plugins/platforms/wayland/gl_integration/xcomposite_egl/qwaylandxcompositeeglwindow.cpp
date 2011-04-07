#include "qwaylandxcompositeeglwindow.h"

#include <QtCore/QDebug>

QWaylandXCompositeEGLWindow::QWaylandXCompositeEGLWindow(QWidget *window, QWaylandXCompositeEGLIntegration *glxIntegration)
    : QWaylandWindow(window)
    , mGlxIntegration(glxIntegration)
    , mContext(0)
{

}

QWaylandWindow::WindowType QWaylandXCompositeEGLWindow::windowType() const
{
    //yeah. this type needs a new name
    return QWaylandWindow::Egl;
}

QPlatformGLContext * QWaylandXCompositeEGLWindow::glContext() const
{
    if (!mContext) {
        qDebug() << "creating glcontext;";
        QWaylandXCompositeEGLWindow *that = const_cast<QWaylandXCompositeEGLWindow *>(this);
        that->mContext = new QWaylandXCompositeEGLContext(mGlxIntegration,that);
    }
    return mContext;
}

void QWaylandXCompositeEGLWindow::setGeometry(const QRect &rect)
{
    QWaylandWindow::setGeometry(rect);

    if (mContext) {
        mContext->geometryChanged();
    }
}
