load(qttest_p4)
SOURCES  += tst_qnativesocketengine.cpp

include(../qnativesocketengine/qsocketengine.pri)

requires(contains(QT_CONFIG,private_tests))

MOC_DIR=tmp

QT = core network

symbian: TARGET.CAPABILITY = NetworkServices

