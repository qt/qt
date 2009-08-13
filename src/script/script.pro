TARGET     = QtScript
QPRO_PWD   = $$PWD
QT         = core
DEFINES   += QT_BUILD_SCRIPT_LIB
DEFINES   += QT_NO_USING_NAMESPACE
DEFINES   += QLALR_NO_QSCRIPTGRAMMAR_DEBUG_INFO
#win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x66000000       ### FIXME

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore

include(../qbase.pri)

# disable JIT for now
DEFINES += ENABLE_JIT=0
# FIXME: shared the statically built JavaScriptCore

# Fetch the base WebKit directory from the WEBKITDIR environment variable;
# fall back to src/3rdparty otherwise
WEBKITDIR = $$(WEBKITDIR)
isEmpty(WEBKITDIR) {
    WEBKITDIR = $$PWD/../3rdparty/webkit

    # FIXME: not needed once JSCBISON works
    # TODO: or leave it like this since the generated file is available anyway?
    SOURCES += $$WEBKITDIR/JavaScriptCore/generated/Grammar.cpp
} else {
    CONFIG += building-libs
    CONFIG -= QTDIR_build
    include($$WEBKITDIR/WebKit.pri)
}

include($$WEBKITDIR/JavaScriptCore/JavaScriptCore.pri)

INCLUDEPATH += $$WEBKITDIR/JavaScriptCore
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/parser
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/bytecompiler
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/debugger
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/runtime
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/wtf
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/unicode
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/interpreter
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/jit
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/profiler
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/wrec
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/API
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/bytecode
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/assembler
INCLUDEPATH += $$WEBKITDIR/JavaScriptCore/generated

DEFINES += BUILDING_QT__=1
DEFINES += USE_SYSTEM_MALLOC
DEFINES += WTF_USE_JAVASCRIPTCORE_BINDINGS=1
DEFINES += WTF_CHANGES=1
DEFINES += NDEBUG

include(script.pri)

