TARGET     = QtScript
QPRO_PWD   = $$PWD
QT         = core
DEFINES   += QT_BUILD_SCRIPT_LIB
DEFINES   += QT_NO_USING_NAMESPACE
#win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x66000000       ### FIXME

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore

include($$PWD/../../qbase.pri)

CONFIG += building-libs

include($$PWD/../v8/v8base.pri)

INCLUDEPATH += $$PWD

INCLUDEPATH += $$V8DIR/include
macx:CONFIG(debug, debug|release) {
    LIBS += -L. -L../v8/ -L../snapshot/ -lv8_debug -lsnapshot_debug
} else {
    LIBS += -L. -L../v8/ -L../snapshot/ -lv8 -lsnapshot
}

# Avoid qmake adding -lv8 et al as dependencies.
CONFIG -= explicitlib

SOURCES += \
    $$PWD/qscriptclass.cpp \
    $$PWD/qscriptclasspropertyiterator.cpp \
    $$PWD/qscriptcontext.cpp \
    $$PWD/qscriptcontextinfo.cpp \
    $$PWD/qscriptengine.cpp \
    $$PWD/qscriptengineagent.cpp \
    $$PWD/qscriptextensionplugin.cpp \
    $$PWD/qscriptprogram.cpp \
    $$PWD/qscriptstring.cpp \
    $$PWD/qscriptsyntaxcheckresult.cpp \
    $$PWD/qscriptvalue.cpp \
    $$PWD/qscriptvalueiterator.cpp \
    $$PWD/qscriptable.cpp

HEADERS += \
    $$PWD/qscriptclass.h \
    $$PWD/qscriptclass_p.h \
    $$PWD/qscriptclass_impl_p.h \
    $$PWD/qscriptclasspropertyiterator.h \
    $$PWD/qscriptcontext.h \
    $$PWD/qscriptcontext_p.h \
    $$PWD/qscriptcontext_impl_p.h \
    $$PWD/qscriptcontextinfo.h \
    $$PWD/qscriptengine.h \
    $$PWD/qscriptengine_p.h \
    $$PWD/qscriptengine_impl_p.h \
    $$PWD/qscriptengineagent.h \
    $$PWD/qscriptengineagent_p.h \
    $$PWD/qscriptengineagent_impl_p.h \
    $$PWD/qscriptextensioninterface.h \
    $$PWD/qscriptextensionplugin.h \
    $$PWD/qscriptprogram.h \
    $$PWD/qscriptprogram_p.h \
    $$PWD/qscriptstring.h \
    $$PWD/qscriptstring_p.h \
    $$PWD/qscriptstring_impl_p.h \
    $$PWD/qscriptsyntaxcheckresult_p.h \
    $$PWD/qscriptvalue.h \
    $$PWD/qscriptvalue_p.h \
    $$PWD/qscriptvalue_impl_p.h \
    $$PWD/qscriptvalueiterator.h \
    $$PWD/qscriptable.h \
    $$PWD/qscriptable_p.h \
    $$PWD/qscriptable_impl_p.h \
    $$PWD/qscriptisolate_p.h \
    $$PWD/qscriptshareddata_p.h \
    $$PWD/qscriptv8objectwrapper_p.h \
    $$PWD/qscripttools_p.h \
    $$PWD/qscript_impl_p.h \

SOURCES += \
    $$PWD/qscriptdeclarativeclass.cpp \
    $$PWD/qscriptoriginalglobalobject_p.cpp \
    $$PWD/qscriptqobject.cpp

HEADERS += \
    $$PWD/qscriptdeclarativeclass_p.h \
    $$PWD/qscriptdeclarativeclassobject_p.h \
    $$PWD/qscriptoriginalglobalobject_p.h \
    $$PWD/qscriptqobject_p.h \
    $$PWD/qscriptqobject_impl_p.h \
    $$PWD/qscriptfunction_p.h

symbian:TARGET.UID3=0x2001B2E1
