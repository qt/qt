TEMPLATE = subdirs
SUBDIRS = car \
	  controller

# install
sources.files = *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/dbus/remotecontrolledcar
INSTALLS += sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)
