TEMPLATE = subdirs
SUBDIRS = \
        access \
        kernel \
        ssl \
        socket

TRUSTED_BENCHMARKS += \
    socket/qtcpserver \
    ssl/qsslsocket

include(../trusted-benchmarks.pri)
