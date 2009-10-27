TEMPLATE = lib
TARGET = QmlInspector

INCLUDEPATH += .
DEPENDPATH += .

include(../standalone/qmldebugger.pri)

HEADERS += qmlinspectorplugin.h \
           qmlinspector.h \
           qmlinspectormode.h \
           inspectoroutputpane.h \
           runcontrol.h

SOURCES += qmlinspectorplugin.cpp \
           qmlinspectormode.cpp \
           inspectoroutputpane.cpp \
           runcontrol.cpp 

OTHER_FILES += QmlInspector.pluginspec
RESOURCES += qmlinspector.qrc

IDE_BUILD_TREE=$$(CREATOR_BUILD_DIR)

include($$(CREATOR_SRC_DIR)/src/qtcreatorplugin.pri)
include($$(CREATOR_SRC_DIR)/src/plugins/projectexplorer/projectexplorer.pri)
include($$(CREATOR_SRC_DIR)/src/plugins/coreplugin/coreplugin.pri)
LIBS += -L$$(CREATOR_BUILD_DIR)/lib/qtcreator

