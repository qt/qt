#include <X11/Xlib.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>

int main(int argc, char** argv)
{
    unsigned int count = 0;
    XvAdaptorInfo *adaptors = 0;
    XvQueryAdaptors(0, 0, &count, &adaptors);
    return 0;
}
