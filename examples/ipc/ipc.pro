TEMPLATE      = subdirs
# no QSharedMemory
!vxworks:!qnx:SUBDIRS = sharedmemory
!wince*: SUBDIRS += localfortuneserver localfortuneclient

# install
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS ipc.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/ipc
INSTALLS += sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)
