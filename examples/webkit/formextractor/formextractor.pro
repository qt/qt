QT          +=  webkit
TARGET       =  formExtractor
TEMPLATE     =  app
SOURCES     +=  main.cpp \
                formextractor.cpp \
                mainwindow.cpp
HEADERS     +=  formextractor.h \
                mainwindow.h
FORMS        += formextractor.ui
RESOURCES    += formextractor.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/formextractor
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro form.html images
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/formextractor
INSTALLS += target sources
