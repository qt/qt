#include <qglobal.h>

#if defined(Q_CC_MSVC) || defined(Q_CC_MSVC_NET) || defined(Q_CC_BOR)|| defined(Q_OS_SYMBIAN)
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif

#if defined(Q_CC_BOR)
# define BORLAND_STDCALL __stdcall
#else
# define BORLAND_STDCALL
#endif

LIB_EXPORT int BORLAND_STDCALL mylibversion()
{
    return 2;
}

