#ifndef QWAYLANDXPIXMAPGLXINTEGRATION_H
#define QWAYLANDXPIXMAPGLXINTEGRATION_H

#include "gl_integration/qwaylandglintegration.h"

#include <QtCore/QTextStream>
#include <QtCore/QDataStream>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>
#include <QtGui/QWidget>

#include <X11/Xlib.h>

class QWaylandXPixmapGLXIntegration : public QWaylandGLIntegration
{
public:
    QWaylandXPixmapGLXIntegration(QWaylandDisplay * waylandDispaly);
    ~QWaylandXPixmapGLXIntegration();

    void initialize();

    QWaylandWindow *createEglWindow(QWidget *widget);

    QWaylandDisplay *waylandDisplay() const;

    Display *xDisplay() const;
    int screen() const;
    Window rootWindow() const;

private:
    QWaylandDisplay *mWaylandDisplay;

    Display *mDisplay;
    int mScreen;
    Window mRootWindow;

};

#endif // QWAYLANDXPIXMAPGLXINTEGRATION_H
