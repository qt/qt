load(qttest_p4)
SOURCES += tst_qgraphicsvideoitem.cpp

QT += multimedia mediaservices
requires(contains(QT_CONFIG, mediaservices))
