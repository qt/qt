load(qttest_p4)
TEMPLATE = app
TARGET = tst_qstringbuilder

# Uncomment to test compilation of the drop-in
# replacement operator+()
#DEFINES += QT_USE_FAST_OPERATOR_PLUS

QMAKE_CXXFLAGS += -g
QMAKE_CFLAGS += -g

QT -= gui

CONFIG += release

SOURCES += main.cpp 
