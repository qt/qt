#ifndef QWAYLANDXCOMPOSITEGLXWINDOW_H
#define QWAYLANDXCOMPOSITEGLXWINDOW_H

#include "qwaylandwindow.h"
#include "qwaylandxcompositeglxintegration.h"
#include "qwaylandxcompositeglxcontext.h"

class QWaylandXCompositeGLXWindow : public QWaylandWindow
{
public:
    QWaylandXCompositeGLXWindow(QWidget *window, QWaylandXCompositeGLXIntegration *glxIntegration);
    WindowType windowType() const;

    QPlatformGLContext *glContext() const;

    void setGeometry(const QRect &rect);

private:
    QWaylandXCompositeGLXIntegration *mGlxIntegration;
    QWaylandXCompositeGLXContext *mContext;

};

#endif // QWAYLANDXCOMPOSITEGLXWINDOW_H
