#ifndef QWAYLANDGLINTEGRATION_H
#define QWAYLANDGLINTEGRATION_H

class QWaylandWindow;
class QWaylandDisplay;
class QWidget;

class QWaylandGLIntegration
{
public:
    QWaylandGLIntegration();
    virtual ~QWaylandGLIntegration();

    virtual void initialize() = 0;

    virtual QWaylandWindow *createEglWindow(QWidget *widget) = 0;

    static QWaylandGLIntegration *createEglIntegration(QWaylandDisplay *waylandDisplay);
};

#endif // QWAYLANDGLINTEGRATION_H
