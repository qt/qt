#ifndef EGLNULLWSSCREEN
#define EGLNULLWSSCREEN

#include <QGLScreen>

class EGLNullWSScreen : public QGLScreen
{
public:
    EGLNullWSScreen(int displayId);
    virtual ~EGLNullWSScreen();

    virtual bool initDevice();
    virtual bool connect(const QString &displaySpec);
    virtual void disconnect();
    virtual void shutdownDevice();

    virtual void setMode(int width, int height, int depth);
    virtual void blank(bool on);

    virtual void exposeRegion(QRegion r, int changing);

    virtual QWSWindowSurface* createSurface(QWidget *widget) const;
    virtual QWSWindowSurface* createSurface(const QString &key) const;

    virtual bool hasOpenGL() { return true; }
};

#endif // EGLNULLWSSCREEN
