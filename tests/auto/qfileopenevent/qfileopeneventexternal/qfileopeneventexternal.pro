TEMPLATE = app
TARGET = qfileopeneventexternal
QT += core gui
SOURCES += qfileopeneventexternal.cpp
symbian: {
    TARGET.UID3 = 0xe9410b39
    MMP_RULES += DEBUGGABLE_UDEBONLY
    RSS_RULES += "embeddability=KAppEmbeddable;"
    RSS_RULES.datatype_list += "priority = EDataTypePriorityHigh; type = \"application/x-tst_qfileopenevent\";"
    LIBS += -lapparc \
        -leikcore -lefsrv -lcone
}
