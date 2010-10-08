QT = core
TEMPLATE = subdirs
SUBDIRS = plugin pluginWrongCase
tst_qdeclarativemoduleplugin_pro.depends += plugin
SUBDIRS += tst_qdeclarativemoduleplugin.pro

CONFIG += parallel_test

