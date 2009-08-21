TEMPLATE      = subdirs

SUBDIRS             =   drilldown
!symbian: SUBDIRS   +=  cachedtable \
                        relationaltablemodel \
                        sqlwidgetmapper

!wince*:!symbian: SUBDIRS += \
                        querymodel \
                        tablemodel \
                        masterdetail

# install
sources.files = connection.h sql.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/sql
INSTALLS += sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)
