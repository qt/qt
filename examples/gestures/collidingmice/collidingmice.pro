HEADERS += \
	mouse.h \
        gesturerecognizerlinjazax.h \
        linjazaxgesture.h

SOURCES += \
	main.cpp \
        mouse.cpp \
        gesturerecognizerlinjazax.cpp

RESOURCES += \
	mice.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/gestures/collidingmice
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS collidingmice.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/gestures/collidingmice
INSTALLS += target sources
