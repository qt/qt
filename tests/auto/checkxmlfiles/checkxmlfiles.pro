CONFIG += testcase

TARGET = tst_checkxmlfiles
QT = core testlib
SOURCES += tst_checkxmlfiles.cpp \
           ../qxmlquery/TestFundament.cpp

include (../xmlpatterns.pri)

wince*|symbian: {
QT += network
addFiles.files = \
	$$QT_SOURCE_TREE/examples/sql/masterdetail/albumdetails.xml \
	$$QT_SOURCE_TREE/examples/xmlpatterns/xquery/globalVariables/globals.gccxml \
	$$QT_SOURCE_TREE/doc/src/diagrams/stylesheet/treeview.svg \
	$$QT_SOURCE_TREE/doc/src/diagrams/designer-manual/designer-mainwindow-actions.ui \
	$$QT_SOURCE_TREE/demos/undo/undo.qrc
addFiles.path = xmlfiles
DEPLOYMENT += addFiles
}
