load(qttest_p4)
SOURCES  += tst_qsocks5socketengine.cpp


include(../qnativesocketengine/qsocketengine.pri)


MOC_DIR=tmp

QT = core network

# Symbian toolchain does not support correct include semantics
symbian:INCPATH+=..\\..\\..\\include\\QtNetwork\\private
symbian: TARGET.CAPABILITY = NetworkServices


requires(contains(QT_CONFIG,private_tests))
