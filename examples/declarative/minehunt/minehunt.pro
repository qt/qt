SOURCES = main.cpp

QT += script declarative

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/minehunt
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS minehunt.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/minehunt
INSTALLS += target sources
