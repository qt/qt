TARGET	   = QtCore
QPRO_PWD   = $$PWD
QT         =
DEFINES   += QT_BUILD_CORE_LIB QT_NO_USING_NAMESPACE
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x67000000
irix-cc*:QMAKE_CXXFLAGS += -no_prelink -ptused

include(../qbase.pri)
include(animation/animation.pri)
include(arch/arch.pri)
include(concurrent/concurrent.pri)
include(global/global.pri)
include(thread/thread.pri)
include(tools/tools.pri)
include(io/io.pri)
include(plugin/plugin.pri)
include(kernel/kernel.pri)
include(codecs/codecs.pri)
include(statemachine/statemachine.pri)
include(xml/xml.pri)

mac|darwin:LIBS_PRIVATE += -framework ApplicationServices

mac:lib_bundle:DEFINES += QT_NO_DEBUG_PLUGIN_CHECK
win32:DEFINES-=QT_NO_CAST_TO_ASCII

QMAKE_LIBS += $$QMAKE_LIBS_CORE

QMAKE_DYNAMIC_LIST_FILE = $$PWD/QtCore.dynlist

contains(DEFINES,QT_EVAL):include(eval.pri)

symbian: {
    TARGET.UID3=0x2001B2DC

    # Workaroud for problems with paging this dll
    MMP_RULES -= PAGED
    MMP_RULES *= UNPAGED

    # Partial upgrade SIS file
    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt\"" \
        " "
    pu_header = "; Partial upgrade package for testing QtCore changes without reinstalling everything" \
                "$${LITERAL_HASH}{\"Qt corelib\"}, (0x2001E61C), $${QT_MAJOR_VERSION},$${QT_MINOR_VERSION},$${QT_PATCH_VERSION}, TYPE=PU"
    partial_upgrade.pkg_prerules = pu_header vendorinfo
    partial_upgrade.sources = $$QMAKE_LIBDIR_QT/QtCore$${QT_LIBINFIX}.dll
    partial_upgrade.path = c:/sys/bin
    DEPLOYMENT = partial_upgrade $$DEPLOYMENT
}

mmx {
    DEFINES += QT_HAVE_MMX
}
3dnow {
    DEFINES += QT_HAVE_3DNOW
}
sse {
    DEFINES += QT_HAVE_SSE
    DEFINES += QT_HAVE_MMXEXT
}
sse2 {
    DEFINES += QT_HAVE_SSE2
}
iwmmxt {
    DEFINES += QT_HAVE_IWMMXT
}
neon {
    DEFINES += QT_HAVE_NEON
    QMAKE_CXXFLAGS *= -mfpu=neon
}

