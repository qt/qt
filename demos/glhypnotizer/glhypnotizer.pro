TEMPLATE = app
CONFIG -= moc
INCLUDEPATH += .

# Input
SOURCES += main.cpp
QT += opengl svg
RESOURCES = hypnotizer.qrc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
target.path = $$[QT_INSTALL_DEMOS]/glhypnotizer
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.png *.pro *.svg
sources.path = $$[QT_INSTALL_DEMOS]/glhypnotizer
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
