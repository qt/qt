############################################################
# Project file for autotest for file qfiledialog.h
############################################################

load(qttest_p4)

SOURCES += tst_qfiledialog.cpp

wince*|symbian: {
    addFiles.sources = *.cpp
    addFiles.path = .
    filesInDir.sources = *.pro
    filesInDir.path = someDir
    DEPLOYMENT += addFiles filesInDir
}

symbian:TARGET.EPOCHEAPSIZE="0x100 0x1000000"
symbian:HEADERS += ../../../include/qtgui/private/qfileinfogatherer_p.h
