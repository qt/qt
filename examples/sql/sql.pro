TEMPLATE      = subdirs

SUBDIRS             =   drilldown
SUBDIRS   +=  cachedtable \
                        relationaltablemodel \
                        sqlwidgetmapper

!wince*:  SUBDIRS   +=  masterdetail

!wince*: SUBDIRS += \
                        querymodel \
                        tablemodel


# install
sources.files = connection.h sql.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/sql
INSTALLS += sources

