QT = core
TEMPLATE = subdirs
SUBDIRS = plugin
tst_qdeclarativemoduleplugin_pro.depends += plugin
SUBDIRS += tst_qdeclarativemoduleplugin.pro

CONFIG += parallel_test

