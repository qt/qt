HEADERS =   softkeys.h
SOURCES += \
            main.cpp \
            softkeys.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/softkeys
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS softkeys.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/softkeys
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian:TARGET.UID3 = 0xA000CF6B
