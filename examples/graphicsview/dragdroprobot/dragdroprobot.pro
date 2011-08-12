HEADERS += \
	coloritem.h \
	robot.h

SOURCES += \
	coloritem.cpp \
	main.cpp \
	robot.cpp

RESOURCES += \
	robot.qrc


# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/dragdroprobot
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS dragdroprobot.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/dragdroprobot
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

simulator: warning(This example might not fully work on Simulator platform)
