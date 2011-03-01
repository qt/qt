#ifndef QDRI2CONTEXT_H
#define QDRI2CONTEXT_H

#include <QtGui/QPlatformGLContext>

class QXcbWindow;
class QDri2ContextPrivate;

struct xcb_dri2_dri2_buffer_t;

class QDri2Context : public QPlatformGLContext
{
    Q_DECLARE_PRIVATE(QDri2Context);
public:
    QDri2Context(QXcbWindow *window);
    ~QDri2Context();

    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void* getProcAddress(const QString& procName);

    void resize(const QSize &size);

    QPlatformWindowFormat platformWindowFormat() const;

protected:
    xcb_dri2_dri2_buffer_t *backBuffer();
    QScopedPointer<QDri2ContextPrivate> d_ptr;
private:
    Q_DISABLE_COPY(QDri2Context)
};

#endif // QDRI2CONTEXT_H
