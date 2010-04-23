TARGET     = QtGui
QPRO_PWD   = $$PWD
QT = core
DEFINES   += QT_BUILD_GUI_LIB QT_NO_USING_NAMESPACE
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x65000000

!win32:!embedded:!mac:!symbian:CONFIG      += x11

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore

include(../qbase.pri)

contains(QT_CONFIG, x11sm):CONFIG += x11sm

#platforms
x11:include(kernel/x11.pri)
mac:include(kernel/mac.pri)
win32:include(kernel/win.pri)
embedded:include(embedded/embedded.pri)
symbian {
    include(kernel/symbian.pri)
    include(s60framework/s60framework.pri)
}

#modules
include(animation/animation.pri)
include(kernel/kernel.pri)
include(image/image.pri)
include(painting/painting.pri)
include(text/text.pri)
include(styles/styles.pri)
include(widgets/widgets.pri)
include(dialogs/dialogs.pri)
include(accessible/accessible.pri)
include(itemviews/itemviews.pri)
include(inputmethod/inputmethod.pri)
include(graphicsview/graphicsview.pri)
include(util/util.pri)
include(statemachine/statemachine.pri)
include(math3d/math3d.pri)
include(effects/effects.pri)

include(egl/egl.pri)
win32:!wince*: DEFINES += QT_NO_EGL
embedded: QT += network

QMAKE_LIBS += $$QMAKE_LIBS_GUI

contains(DEFINES,QT_EVAL):include($$QT_SOURCE_TREE/src/corelib/eval.pri)

QMAKE_DYNAMIC_LIST_FILE = $$PWD/QtGui.dynlist

DEFINES += Q_INTERNAL_QAPP_SRC
symbian {
    TARGET.UID3=0x2001B2DD

    symbian-abld|symbian-sbsv2 {
        # ro-section in gui can exceed default allocated space, so move rw-section a little further
        QMAKE_LFLAGS.ARMCC += --rw-base 0x800000
        QMAKE_LFLAGS.GCCE += -Tdata 0xC00000
    }

    # Partial upgrade SIS file
    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt\"" \
        " "
    pu_header = "; Partial upgrade package for testing QtGui changes without reinstalling everything" \
                "$${LITERAL_HASH}{\"Qt gui\"}, (0x2001E61C), $${QT_MAJOR_VERSION},$${QT_MINOR_VERSION},$${QT_PATCH_VERSION}, TYPE=PU"
    partial_upgrade.pkg_prerules = pu_header vendorinfo
    partial_upgrade.sources = $$QMAKE_LIBDIR_QT/QtGui.dll
    partial_upgrade.path = c:/sys/bin
    DEPLOYMENT = partial_upgrade $$DEPLOYMENT
}

