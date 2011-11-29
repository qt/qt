TARGET = qsymsql
PLUGIN_TYPE = sqldrivers
SOURCES = main.cpp

include(../../../sql/drivers/symsql/qsql_symsql.pri)
include(../qsqldriverbase.pri)

symbian: {
pluginDep.sources = $${TARGET}.dll 
pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE} 
DEPLOYMENT += pluginDep       
}

