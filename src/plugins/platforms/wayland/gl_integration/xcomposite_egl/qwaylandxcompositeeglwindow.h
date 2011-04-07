#ifndef QWAYLANDXCOMPOSITEEGLWINDOW_H
#define QWAYLANDXCOMPOSITEEGLWINDOW_H

#include "qwaylandwindow.h"
#include "qwaylandxcompositeeglintegration.h"
#include "qwaylandxcompositeeglcontext.h"

class QWaylandXCompositeEGLWindow : public QWaylandWindow
{
public:
    QWaylandXCompositeEGLWindow(QWidget *window, QWaylandXCompositeEGLIntegration *glxIntegration);
    WindowType windowType() const;

    QPlatformGLContext *glContext() const;

    void setGeometry(const QRect &rect);

private:
    QWaylandXCompositeEGLIntegration *mGlxIntegration;
    QWaylandXCompositeEGLContext *mContext;

};

#endif // QWAYLANDXCOMPOSITEEGLWINDOW_H
