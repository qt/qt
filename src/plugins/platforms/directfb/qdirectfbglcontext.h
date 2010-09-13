#ifndef QDIRECTFBGLCONTEXT_H
#define QDIRECTFBGLCONTEXT_H

#include <QPlatformGLContext>

#include "qdirectfbconvenience.h"

class QDirectFbGLContext : public QPlatformGLContext
{
public:
    explicit QDirectFbGLContext(IDirectFBGL *glContext);

    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void *getProcAddress(const QString &procName);

    QPlatformWindowFormat platformWindowFormat() const;


private:
    IDirectFBGL *m_dfbGlContext;

    QPlatformWindowFormat m_windowFormat;

};

#endif // QDIRECTFBGLCONTEXT_H
