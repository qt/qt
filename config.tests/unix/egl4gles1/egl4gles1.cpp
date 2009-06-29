#include <GLES/egl.h>

int main(int, char **)
{
    EGLint x = 0;
    EGLDisplay dpy = 0;
    EGLContext ctx = 0;
    eglDestroyContext(dpy, ctx);
    return 0;
}
