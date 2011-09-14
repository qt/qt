QT += script
CONFIG += uitools

HEADERS = tetrixboard.h
SOURCES = main.cpp \
	  tetrixboard.cpp

RESOURCES = tetrix.qrc

contains(QT_CONFIG, scripttools): QT += scripttools

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script/qstetrix
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS qstetrix.pro *.js
sources.path = $$[QT_INSTALL_EXAMPLES]/script/qstetrix
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example does not work on Symbian platform)
