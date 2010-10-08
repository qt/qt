load(qttest_p4)
TEMPLATE = app
TARGET = tst_compilation
QT += declarative
macx:CONFIG -= app_bundle

CONFIG += release

SOURCES += tst_compilation.cpp

symbian {
  data.files += data
  data.path = .
  DEPLOYMENT += data
} else {
  DEFINES += SRCDIR=\\\"$$PWD\\\"
}
