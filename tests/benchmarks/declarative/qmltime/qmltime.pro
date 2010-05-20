load(qttest_p4)
TEMPLATE = app
TARGET = qmltime
QT += declarative
macx:CONFIG -= app_bundle

SOURCES += qmltime.cpp 

symbian {
    TARGET.CAPABILITY = "All -TCB"
    example.sources = example.qml tests
    example.path = .
    DEPLOYMENT += example
}

