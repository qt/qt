CONFIG += qttest_p4

QT = core gui

SOURCES		+= tst_qfilesystemmodel.cpp
TARGET		= tst_qfilesystemmodel

symbian: {
    HEADERS += ../../../include/qtgui/private/qfileinfogatherer_p.h

    # need to deploy something to create the private directory
    dummyDeploy.sources = tst_qfilesystemmodel.cpp
    dummyDeploy.path = .
    DEPLOYMENT += dummyDeploy
    LIBS += -lefsrv
}
