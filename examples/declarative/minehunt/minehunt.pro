SOURCES = main.cpp

QT += script declarative
contains(QT_CONFIG, opengles2)|contains(QT_CONFIG, opengles1): QT += opengl

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/minehunt
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS minehunt.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/minehunt
INSTALLS += target sources
