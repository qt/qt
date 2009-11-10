load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative webkit network
macx:CONFIG -= app_bundle

HEADERS += testhttpserver.h

SOURCES += tst_xmlhttprequest.cpp \
           testhttpserver.cpp


# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"
