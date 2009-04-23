QT      +=  network
SOURCES =   main.cpp searchbox.cpp googlesuggest.cpp
HEADERS  =  searchbox.h googlesuggest.h

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/googlesuggest
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/googlesuggest
INSTALLS += target sources
