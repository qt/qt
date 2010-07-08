#ifndef EGLNULLWSWINDOWSURFACE_H
#define EGLNULLWSWINDOWSURFACE_H

#include <private/qglwindowsurface_qws_p.h>

class EGLNullWSWindowSurface : public QWSGLWindowSurface
{
public:
    EGLNullWSWindowSurface(QWidget *widget);
    EGLNullWSWindowSurface();
    virtual ~EGLNullWSWindowSurface();

    virtual QString key() const;
    virtual QPaintDevice *paintDevice();
    virtual bool isValid() const;
    virtual QImage image() const;

private:
    QWidget *widget;
};

#endif // EGLNULLWSWINDOWSURFACE_H
