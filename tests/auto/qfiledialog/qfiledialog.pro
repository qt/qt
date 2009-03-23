############################################################
# Project file for autotest for file qfiledialog.h
############################################################

load(qttest_p4)

SOURCES += tst_qfiledialog.cpp

wince*: {
    addFiles.sources = *.cpp
    addFiles.path = .
    filesInDir.sources = *.pro
    filesInDir.path = someDir
    DEPLOYMENT += addFiles filesInDir
}
