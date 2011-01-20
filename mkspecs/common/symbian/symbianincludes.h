#ifndef __PRODUCT_INCLUDE__
#  ifdef __QT_PRODUCT_INCLUDE_IS_LOWERCASE__
#    define __PRODUCT_INCLUDE__ <variant/symbian_os.hrh>
#  else
#    define __PRODUCT_INCLUDE__ <variant/Symbian_OS.hrh>
#  endif
#endif

#ifndef __QT_SYMBIAN_RESOURCE__
#  if defined(__ARMCC__) || defined(__CC_ARM)
#    ifdef __QT_RVCT_HEADER_IS_2_2__
#      include <rvct2_2.h>
#    else
#      include <rvct.h>
#    endif
#  endif
#endif
