#ifndef QWAYLANDXPIXMAPWINDOW_H
#define QWAYLANDXPIXMAPWINDOW_H

#include "qwaylandshmwindow.h"
#include "qwaylandxpixmapeglintegration.h"

class QXPixmapReadbackGLContext;

class QWaylandXPixmapWindow : public QWaylandShmWindow
{
public:
    QWaylandXPixmapWindow(QWidget *window, QWaylandXPixmapEglIntegration *eglIntegration);

    WindowType windowType() const;

    QPlatformGLContext *glContext() const;

    void setGeometry(const QRect &rect);

private:
    QWaylandXPixmapEglIntegration *mEglIntegration;
    QXPixmapReadbackGLContext *mContext;
};

#endif // QWAYLANDXPIXMAPWINDOW_H
