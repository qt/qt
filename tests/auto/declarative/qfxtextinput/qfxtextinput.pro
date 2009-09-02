load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative gui
SOURCES += tst_qfxtextinput.cpp

# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"
