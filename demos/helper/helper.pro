TEMPLATE = subdirs

# the helpers simply need a make install target, but require no compilation 
sources.files = \
	qmlapplicationviewer
sources.path = $$[QT_INSTALL_DEMOS]/helper
INSTALLS += sources
