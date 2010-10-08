load(qttest_p4)
SOURCES += tst_patternistexamples.cpp
CONFIG += qtestlib
wince*|symbian: {
	snippets.files = $$QT_SOURCE_TREE/doc/src/snippets/patternist/*
	snippets.path = patternist
	widgetRen.files = $$QT_SOURCE_TREE/examples/xmlpatterns/xquery/widgetRenderer/*
	widgetRen.path = widgetRenderer
	globVar.files = $$QT_SOURCE_TREE/examples/xmlpatterns/xquery/globalVariables/*
	globVar.path = globalVariables
	filetree.files = $$QT_SOURCE_TREE/examples/xmlpatterns/filetree/*
	filetree.path = filetree
	recipes.files = $$QT_SOURCE_TREE/examples/xmlpatterns/recipes/*
	recipes.path = recipes
	files.files = $$QT_SOURCE_TREE/examples/xmlpatterns/recipes/files/*
	files.path = recipes\\files

	DEPLOYMENT += snippets widgetRen globVar filetree recipes files
	# take care of dependency
	QT += network
}
include (../xmlpatterns.pri)
