load(qttest_p4)
TEMPLATE = app
TARGET = tst_qgraphicsview

SOURCES += tst_qgraphicsview.cpp
RESOURCES += qgraphicsview.qrc

include(chiptester/chiptester.pri)
