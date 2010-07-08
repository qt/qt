#ifndef EGLNULLWSSCREEN
#define EGLNULLWSSCREEN

#include <QGLScreen>

class EGLNullWSScreen : public QGLScreen
{
public:
    EGLNullWSScreen(int displayId);
    ~EGLNullWSScreen();

    bool initDevice();
    bool connect(const QString &displaySpec);
    void disconnect();
    void shutdownDevice();

    void setMode(int width, int height, int depth);
    void blank(bool on);

    void exposeRegion(QRegion r, int changing);

    QWSWindowSurface* createSurface(QWidget *widget) const;
    QWSWindowSurface* createSurface(const QString &key) const;

    bool hasOpenGL() { return true; }
};

#endif // EGLNULLWSSCREEN
