load(qttest_p4)
symbian:TARGET.EPOCHEAPSIZE=0x200000 0xa00000

SOURCES += tst_qtwidgets.cpp mainwindow.cpp
HEADERS += mainwindow.h
QT += network
RESOURCES = qtstyles.qrc
FORMS += advanced.ui system.ui standard.ui


