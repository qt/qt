load(qttest_p4)
SOURCES  += tst_qhttpnetworkconnection.cpp
INCLUDEPATH += $$(QTDIR)/src/3rdparty/zlib

requires(contains(QT_CONFIG,private_tests))

QT = core network
