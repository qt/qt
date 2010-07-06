load(qttest_p4)
SOURCES += tst_patternistexamples.cpp
CONFIG += qtestlib
wince*|symbian: {
	snippets.sources = $$QT_SOURCE_TREE/doc/src/snippets/patternist/*
	snippets.path = patternist
	widgetRen.sources = $$QT_SOURCE_TREE/examples/xmlpatterns/xquery/widgetRenderer/*
	widgetRen.path = widgetRenderer
	globVar.sources = $$QT_SOURCE_TREE/examples/xmlpatterns/xquery/globalVariables/*
	globVar.path = globalVariables
	filetree.sources = $$QT_SOURCE_TREE/examples/xmlpatterns/filetree/*
	filetree.path = filetree
	recipes.sources = $$QT_SOURCE_TREE/examples/xmlpatterns/recipes/*
	recipes.path = recipes
	files.sources = $$QT_SOURCE_TREE/examples/xmlpatterns/recipes/files/*
	files.path = recipes\\files

	DEPLOYMENT += snippets widgetRen globVar filetree recipes files
	# take care of dependency
	QT += network
}
include (../xmlpatterns.pri)
