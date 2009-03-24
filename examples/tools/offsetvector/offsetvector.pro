HEADERS       = randomlistmodel.h
SOURCES       = randomlistmodel.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/offsetvector
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS offsetvector.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/offsetvector
INSTALLS += target sources
