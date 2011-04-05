load(qttest_p4)
TEMPLATE = app
TARGET = tst_holistic
QT += declarative script network
macx:CONFIG -= app_bundle

CONFIG += release

SOURCES += tst_holistic.cpp \
           testtypes.cpp
HEADERS += testtypes.h

symbian {
  data.files += data
  data.path = .
  DEPLOYMENT += data
} else {
  DEFINES += SRCDIR=\\\"$$PWD\\\"
}
