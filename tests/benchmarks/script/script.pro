TEMPLATE = subdirs
SUBDIRS = \
        qscriptclass \
        qscriptengine \
        qscriptvalue

TRUSTED_BENCHMARKS += \
    qscriptclass \
    qscriptvalue \
    qscriptengine

include(../trusted-benchmarks.pri)