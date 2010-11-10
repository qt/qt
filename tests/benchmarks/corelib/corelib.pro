TEMPLATE = subdirs
SUBDIRS = \
        io \
        kernel \
        thread \
        tools \
        codecs \
        plugin

TRUSTED_BENCHMARKS += \
    kernel/qmetaobject \
    kernel/qmetatype \
    kernel/qobject \
    thread/qthreadstorage

include(../trusted-benchmarks.pri)