# disable JIT for now
DEFINES += ENABLE_JIT=0
# FIXME: shared the statically built JavaScriptCore
include($$PWD/../3rdparty/webkit/JavaScriptCore/JavaScriptCore.pri)

INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/parser
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/bytecompiler
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/debugger
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/runtime
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/wtf
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/unicode
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/interpreter
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/jit
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/profiler
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/wrec
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/API
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/bytecode
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/assembler
INCLUDEPATH += $$PWD/../3rdparty/webkit/JavaScriptCore/generated


# FIXME: not needed once JSCBISON works
# TODO: or leave it like this since the generated file is available anyway?
SOURCES += $$PWD/../3rdparty/webkit/JavaScriptCore/generated/Grammar.cpp

DEFINES += BUILDING_QT__=1
DEFINES += USE_SYSTEM_MALLOC
DEFINES += WTF_USE_JAVASCRIPTCORE_BINDINGS=1
DEFINES += WTF_CHANGES=1
DEFINES += NDEBUG
