load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
contains(QT_CONFIG, opengles2)|contains(QT_CONFIG, opengles1): QT += opengl
SOURCES += tst_simplecanvasitem.cpp

# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"
