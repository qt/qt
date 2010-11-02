TEMPLATE = subdirs
SUBDIRS = \
        qscriptclass \
        qscriptengine \
        qscriptvalue \
        sunspider \
        v8

TRUSTED_BENCHMARKS += \
    qscriptclass \
    qscriptvalue \
    qscriptengine

include(../trusted-benchmarks.pri)