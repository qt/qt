#include <vg/openvg.h>

int main(int, char **)
{
    VGint i;
    i = 2;
    vgFlush();
    vgDestroyContextSH();
    return 0;
}
