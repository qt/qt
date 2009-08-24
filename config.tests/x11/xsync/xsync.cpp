#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/sync.h>

int main(int, char **)
{
    XSyncValue value;
    (void*)&XSyncIntToValue;
    (void*)&XSyncCreateCounter;
    return 0;
}
