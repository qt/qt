load(qttest_p4)

SOURCES = tst_qplugin.cpp
QT = core

wince*: {
   plugins.sources = plugins/*
   plugins.path = plugins
   DEPLOYMENT += plugins
}

symbian: {
    rpDep.sources = releaseplugin.dll debugplugin.dll
    rpDep.path = plugins
    DEPLOYMENT += rpDep dpDep
}
