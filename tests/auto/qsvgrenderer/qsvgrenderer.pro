############################################################
# Project file for autotest for file qsvgrenderer.h
############################################################

load(qttest_p4)
QT += svg

SOURCES += tst_qsvgrenderer.cpp
RESOURCES += resources.qrc

wince*: {
   addFiles.sources = *.svg *.svgz
   addFiles.path = .

   DEPLOYMENT += addFiles
   DEPLOYMENT_PLUGIN += qsvg
}

