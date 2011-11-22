TEMPLATE = app
TARGET = shadereffects
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative opengl

# Input
SOURCES += main.cpp

symbian {
    DEFINES += SHADEREFFECTS_USE_OPENGL_GRAPHICSSYSTEM
}


target.path = $$[QT_INSTALL_EXAMPLES]/declarative/shadereffects
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS shadereffects.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/shadereffects
qmlfiles.files = qml
qmlfiles.path = $$[QT_INSTALL_EXAMPLES]/declarative/shadereffects

INSTALLS += target sources qmlfiles
