load(qttest_p4)
SOURCES  += tst_qsocks5socketengine.cpp


include(../qnativesocketengine/qsocketengine.pri)


MOC_DIR=tmp

QT = core network



requires(contains(QT_CONFIG,private_tests))
