TARGET = qmldbg_inspector
QT       += declarative

include(../../qpluginbase.pri)

QTDIR_build:DESTDIR  = $$QT_BUILD_TREE/plugins/qmltooling
QTDIR_build:REQUIRES += "contains(QT_CONFIG, declarative)"

SOURCES += \
    abstractviewinspector.cpp \
    qdeclarativeinspectorplugin.cpp \
    qdeclarativeviewinspector.cpp \
    editor/abstractliveedittool.cpp \
    editor/liveselectiontool.cpp \
    editor/livelayeritem.cpp \
    editor/livesingleselectionmanipulator.cpp \
    editor/liverubberbandselectionmanipulator.cpp \
    editor/liveselectionrectangle.cpp \
    editor/liveselectionindicator.cpp \
    editor/boundingrecthighlighter.cpp \
    editor/subcomponentmasklayeritem.cpp \
    editor/zoomtool.cpp \
    editor/colorpickertool.cpp \
    abstracttool.cpp

HEADERS += \
    abstractviewinspector.h \
    qdeclarativeinspectorplugin.h \
    qdeclarativeinspectorprotocol.h \
    qdeclarativeviewinspector.h \
    qdeclarativeviewinspector_p.h \
    qmlinspectorconstants.h \
    editor/abstractliveedittool.h \
    editor/liveselectiontool.h \
    editor/livelayeritem.h \
    editor/livesingleselectionmanipulator.h \
    editor/liverubberbandselectionmanipulator.h \
    editor/liveselectionrectangle.h \
    editor/liveselectionindicator.h \
    editor/boundingrecthighlighter.h \
    editor/subcomponentmasklayeritem.h \
    editor/zoomtool.h \
    editor/colorpickertool.h \
    abstracttool.h

target.path += $$[QT_INSTALL_PLUGINS]/qmltooling
INSTALLS += target

symbian:TARGET.UID3=0x20031E90
