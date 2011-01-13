############################################################
# Project file for autotest for file qnetworkproxy.h (proxy factory part)
############################################################

load(qttest_p4)
QT = core network

symbian: SOURCES += tst_qnetworkproxyfactory_symbian.cpp

symbian: TARGET.CAPABILITY = NetworkServices

