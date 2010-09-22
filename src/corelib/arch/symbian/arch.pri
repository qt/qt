#
# Symbian architecture
#
SOURCES +=  $$QT_ARCH_CPP/qatomic_symbian.cpp \
            $$QT_ARCH_CPP/../armv6/qatomic_generic_armv6.cpp \
            $$QT_ARCH_CPP/heap_hybrid.cpp \
            $$QT_ARCH_CPP/debugfunction.cpp \
            $$QT_ARCH_CPP/qt_heapsetup_symbian.cpp

HEADERS +=  $$QT_ARCH_CPP/dla_p.h \
            $$QT_ARCH_CPP/heap_hybrid_p.h \
            $$QT_ARCH_CPP/common_p.h \
            $$QT_ARCH_CPP/page_alloc_p.h \
            $$QT_ARCH_CPP/slab_p.h

exists($$EPOCROOT/epoc32/include/u32std.h):DEFINES += QT_SYMBIAN_HAVE_U32STD_H
