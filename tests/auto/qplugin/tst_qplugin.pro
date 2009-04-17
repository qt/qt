SOURCES = tst_qplugin.cpp
QT = core
CONFIG += qtestlib

wince*: {
   plugins.sources = plugins/*
   plugins.path = plugins
   DEPLOYMENT += plugins
}

