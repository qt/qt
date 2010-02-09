load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative gui network
macx:CONFIG -= app_bundle

SOURCES += tst_qmlgraphicstextedit.cpp ../shared/testhttpserver.cpp
HEADERS += ../shared/testhttpserver.h

# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"
