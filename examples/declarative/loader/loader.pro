SOURCES = main.cpp
RESOURCES = loader.qrc

QT += script declarative network

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/loader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS loader.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/loader
INSTALLS += target sources
