load(qttest_p4)
QT += network
SOURCES  += tst_qnetworkcachemetadata.cpp

symbian: TARGET.CAPABILITY = NetworkServices

