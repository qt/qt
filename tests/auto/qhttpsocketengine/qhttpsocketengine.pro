load(qttest_p4)
SOURCES  += tst_qhttpsocketengine.cpp


include(../qnativesocketengine/qsocketengine.pri)

MOC_DIR=tmp

QT = core network

symbian: TARGET.CAPABILITY = NetworkServices


