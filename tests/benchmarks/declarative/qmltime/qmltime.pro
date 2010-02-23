load(qttest_p4)
TEMPLATE = app
TARGET = qmltime
QT += declarative
macx:CONFIG -= app_bundle

SOURCES += qmltime.cpp 

symbian* {
    TARGET.CAPABILITY = "All -TCB"
    example.sources = example.qml
    esample.path = .
    tests.sources = tests/*
    tests.path = tests
    anshors.sources = tests/anchors/*
    anchors.path = tests/anchors
    item_creation.sources = tests/item_creation/*
    item_creation.path = tests/item_creation
    positioner_creation.sources = tests/positioner_creation/*
    positioner_creation.path = tests/positioner_creation
    DEPLOYMENT += example tests anchors item_creation positioner_creation
}

