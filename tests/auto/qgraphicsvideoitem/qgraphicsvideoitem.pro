load(qttest_p4)
SOURCES += tst_qgraphicsvideoitem.cpp

QT += mediaservices
requires(contains(QT_CONFIG, mediaservices))
