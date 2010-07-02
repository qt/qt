load(qttest_p4)
SOURCES += tst_checkxmlfiles.cpp \
           ../qxmlquery/TestFundament.cpp
CONFIG += qtestlib
QT -= gui

include (../xmlpatterns.pri)

wince*|symbian: {
QT += network
addFiles.sources = \
	$$QT_SOURCE_TREE/examples/sql/masterdetail/albumdetails.xml \
	$$QT_SOURCE_TREE/examples/xmlpatterns/xquery/globalVariables/globals.gccxml \
	$$QT_SOURCE_TREE/doc/src/diagrams/stylesheet/treeview.svg \
	$$QT_SOURCE_TREE/doc/src/diagrams/designer-manual/designer-mainwindow-actions.ui \
	$$QT_SOURCE_TREE/demos/undo/undo.qrc
addFiles.path = xmlfiles
DEPLOYMENT += addFiles
}
