load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_qfxwebview.cpp

# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"
