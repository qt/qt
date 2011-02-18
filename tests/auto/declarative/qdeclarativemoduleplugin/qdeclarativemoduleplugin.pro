QT = core
TEMPLATE = subdirs
SUBDIRS = plugin plugin.2 plugin.2.1 pluginWrongCase pluginWithQmlFile pluginMixed pluginVersion
tst_qdeclarativemoduleplugin_pro.depends += plugin
SUBDIRS += tst_qdeclarativemoduleplugin.pro

CONFIG += parallel_test

