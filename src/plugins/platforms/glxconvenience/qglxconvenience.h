#ifndef QGLXCONVENIENCE_H
#define QGLXCONVENIENCE_H

#include <QPlatformWindowFormat>

#include <X11/Xlib.h>
#include <GL/glx.h>

XVisualInfo *findVisualInfo(const Display *display, int screen, const QPlatformWindowFormat &format);
GLXFBConfig findConfig(Display *display, int screen, const QPlatformWindowFormat &format);
QPlatformWindowFormat platformWindowFromGLXFBConfig(Display *display, GLXFBConfig config, GLXContext context);
QVector<int> buildSpec(const QPlatformWindowFormat &format);
QPlatformWindowFormat reducePlatformWindowFormat(const QPlatformWindowFormat &format, bool *reduced);

#endif // QGLXCONVENIENCE_H
