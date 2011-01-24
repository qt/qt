HEADERS       = settingswidget.h \
                plotwidget.h
SOURCES       = settingswidget.cpp \
                plotwidget.cpp \
                main.cpp

QT += webkit

# install
target.path = $$[QT_INSTALL_EXAMPLES]/scroller/plot
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS plot.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/scroller/plot
INSTALLS += target sources
