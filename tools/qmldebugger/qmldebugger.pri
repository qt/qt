QT += network declarative
contains(QT_CONFIG, opengles2)|contains(QT_CONFIG, opengles1): QT += opengl

# Input
HEADERS += $$PWD/qmldebugger.h \
           $$PWD/canvasframerate.h \
           $$PWD/watchtable.h \
           $$PWD/engine.h \
           $$PWD/objecttree.h \
           $$PWD/objectpropertiesview.h \
           $$PWD/expressionquerywidget.h

SOURCES += $$PWD/qmldebugger.cpp \
           $$PWD/main.cpp \
           $$PWD/canvasframerate.cpp \
           $$PWD/watchtable.cpp \
           $$PWD/engine.cpp \
           $$PWD/objecttree.cpp \
           $$PWD/objectpropertiesview.cpp \
           $$PWD/expressionquerywidget.cpp

RESOURCES += $$PWD/qmldebugger.qrc

OTHER_FILES += $$PWD/engines.qml
