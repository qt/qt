CONFIG += testcase

TARGET = tst_qfilesystemmodel
QT += testlib
SOURCES += tst_qfilesystemmodel.cpp

symbian: {
    HEADERS += ../../../include/qtgui/private/qfileinfogatherer_p.h

    # need to deploy something to create the private directory
    dummyDeploy.files = tst_qfilesystemmodel.cpp
    dummyDeploy.path = .
    DEPLOYMENT += dummyDeploy
    LIBS += -lefsrv
}
