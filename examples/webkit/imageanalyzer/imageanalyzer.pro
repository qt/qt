TEMPLATE = app
HEADERS = imageanalyzer.h \
	mainwindow.h
SOURCES = imageanalyzer.cpp \
	main.cpp \
	mainwindow.cpp

QT += network webkit

RESOURCES = resources/imageanalyzer.qrc

OTHER_FILES += html/index.html README ../webkit-bridge-tutorial.qdoc outline.txt


