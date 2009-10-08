SOURCES = main.cpp
RESOURCES = dial.qrc

QT += script declarative

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/dial
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS dial.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/dial
INSTALLS += target sources
