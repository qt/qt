// There is some variation in OpenVG engines as to what case
// the VG includes use.  The Khronos reference implementation
// for OpenVG 1.1 uses upper case, so we treat that as canonical.
#if defined(QT_LOWER_CASE_VG_INCLUDES)
#include <vg/openvg.h>
#else
#include <VG/openvg.h>
#endif

int main(int, char **)
{
    VGint i;
    i = 2;
    vgFlush();
    return 0;
}
