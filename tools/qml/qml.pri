QT += declarative script network sql
contains(QT_CONFIG, opengl) {
    QT += opengl
    DEFINES += GL_SUPPORTED
}

INCLUDEPATH += $$PWD

HEADERS += $$PWD/qmlruntime.h \
           $$PWD/proxysettings.h \
           $$PWD/qdeclarativetester.h \
           $$PWD/deviceorientation.h \
           $$PWD/loggerwidget.h
SOURCES += $$PWD/qmlruntime.cpp \
           $$PWD/proxysettings.cpp \
           $$PWD/qdeclarativetester.cpp \
           $$PWD/loggerwidget.cpp

RESOURCES = $$PWD/qmlruntime.qrc
maemo5 {
    SOURCES += $$PWD/deviceorientation_maemo.cpp
} else {
    SOURCES += $$PWD/deviceorientation.cpp
}

symbian {
    INCLUDEPATH += $$QT_SOURCE_TREE/examples/network/qftp/
    LIBS += -lesock -lcommdb -lconnmon -linsock
}

FORMS = $$PWD/recopts.ui \
        $$PWD/proxysettings.ui
