#include <X11/Xlib.h>

int main(int, char **)
{
    Display *d = XOpenDisplay(NULL);
    XCloseDisplay(d);
    return 0;
}

