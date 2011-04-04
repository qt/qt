#ifndef QGLXCONVENIENCE_H
#define QGLXCONVENIENCE_H

#include <QPlatformWindowFormat>

#include <X11/Xlib.h>
#include <GL/glx.h>

XVisualInfo *qglx_findVisualInfo(Display *display, int screen, const QPlatformWindowFormat &format);
GLXFBConfig qglx_findConfig(Display *display, int screen, const QPlatformWindowFormat &format, int drawableBit = GLX_WINDOW_BIT);
QPlatformWindowFormat qglx_platformWindowFromGLXFBConfig(Display *display, GLXFBConfig config, GLXContext context);
QVector<int> qglx_buildSpec(const QPlatformWindowFormat &format, int drawableBit = GLX_WINDOW_BIT);
QPlatformWindowFormat qglx_reducePlatformWindowFormat(const QPlatformWindowFormat &format, bool *reduced);

#endif // QGLXCONVENIENCE_H
