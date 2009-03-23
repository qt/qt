SOURCES += main.cpp filetree.cpp mainwindow.cpp ../shared/xmlsyntaxhighlighter.cpp
HEADERS += filetree.h  mainwindow.h ../shared/xmlsyntaxhighlighter.h
FORMS += forms/mainwindow.ui
QT += xmlpatterns
CONFIG -= app_bundle
RESOURCES += queries.qrc
INCLUDEPATH += ../shared/

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/filetree
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.xq *.html
sources.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns/filetree
INSTALLS += target sources
