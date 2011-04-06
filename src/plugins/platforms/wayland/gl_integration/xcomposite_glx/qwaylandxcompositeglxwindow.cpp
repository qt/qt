#include "qwaylandxcompositeglxwindow.h"

#include <QtCore/QDebug>

QWaylandXCompositeGLXWindow::QWaylandXCompositeGLXWindow(QWidget *window, QWaylandXCompositeGLXIntegration *glxIntegration)
    : QWaylandWindow(window)
    , mGlxIntegration(glxIntegration)
    , mContext(0)
{

}

QWaylandWindow::WindowType QWaylandXCompositeGLXWindow::windowType() const
{
    //yeah. this type needs a new name
    return QWaylandWindow::Egl;
}

QPlatformGLContext * QWaylandXCompositeGLXWindow::glContext() const
{
    if (!mContext) {
        qDebug() << "creating glcontext;";
        QWaylandXCompositeGLXWindow *that = const_cast<QWaylandXCompositeGLXWindow *>(this);
        that->mContext = new QWaylandXCompositeGLXContext(mGlxIntegration,that);
    }
    return mContext;
}

void QWaylandXCompositeGLXWindow::setGeometry(const QRect &rect)
{
    QWaylandWindow::setGeometry(rect);

    if (mContext) {
        mContext->geometryChanged();
    }
}
