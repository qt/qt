#
# Symbian architecture
#
SOURCES +=  $$QT_ARCH_CPP/qatomic_symbian.cpp \
            $$QT_ARCH_CPP/../armv6/qatomic_generic_armv6.cpp \
            $$QT_ARCH_CPP/newallocator.cpp

HEADERS +=  $$QT_ARCH_CPP/dla_p.h \
            $$QT_ARCH_CPP/newallocator_p.h
exists($$EPOCROOT/epoc32/include/u32std.h):DEFINES += QT_SYMBIAN_HAVE_U32STD_H
