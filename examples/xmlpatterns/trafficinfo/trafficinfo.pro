QT          += xmlpatterns
HEADERS     = mainwindow.h stationdialog.h stationquery.h timequery.h
SOURCES     = main.cpp mainwindow.cpp stationdialog.cpp stationquery.cpp timequery.cpp
FORMS       = stationdialog.ui

target.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/trafficinfo
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/trafficinfo
INSTALLS += target sources
