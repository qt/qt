HEADERS = knob.h
SOURCES = main.cpp knob.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/touch/knobs
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS knobs.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/touch/knobs
INSTALLS += target sources
