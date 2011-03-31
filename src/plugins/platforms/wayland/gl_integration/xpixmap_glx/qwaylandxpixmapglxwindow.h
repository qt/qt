#ifndef QWAYLANDXPIXMAPGLXWINDOW_H
#define QWAYLANDXPIXMAPGLXWINDOW_H

#include "qwaylandshmwindow.h"
#include "qwaylandxpixmapglxintegration.h"
#include "qwaylandxpixmapglxcontext.h"

class QWaylandXPixmapGLXWindow : public QWaylandShmWindow
{
public:
    QWaylandXPixmapGLXWindow(QWidget *window, QWaylandXPixmapGLXIntegration *glxIntegration);
    WindowType windowType() const;

    QPlatformGLContext *glContext() const;

    void setGeometry(const QRect &rect);

private:
    QWaylandXPixmapGLXIntegration *mGlxIntegration;
    QWaylandXPixmapGLXContext *mContext;

};

#endif // QWAYLANDXPIXMAPGLXWINDOW_H
