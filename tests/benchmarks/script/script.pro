TEMPLATE = subdirs
SUBDIRS = \
        context2d \
        qscriptclass \
        qscriptengine \
        qscriptvalue \
        sunspider \
        qscriptqobject \
        qscriptvalueiterator \
        v8

TRUSTED_BENCHMARKS += \
    qscriptclass \
    qscriptvalue \
    qscriptengine

include(../trusted-benchmarks.pri)
