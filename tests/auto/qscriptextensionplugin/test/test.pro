load(qttest_p4)

QT = core script
SOURCES = ../tst_qscriptextensionplugin.cpp
CONFIG -= app_bundle
symbian {
    LIBS += -lstaticplugin.lib
    simplePlugin.files = qscriptextension_simpleplugin.dll
    simplePlugin.path = plugins/script
    DEPLOYMENT += simplePlugin
} else {
    LIBS += -L../plugins/script -lstaticplugin
}

TARGET = tst_qscriptextensionplugin
CONFIG(debug_and_release) {
  CONFIG(debug, debug|release) {
    DESTDIR = ../debug
  } else {
    DESTDIR = ../release
  }
} else {
  DESTDIR = ..
}
