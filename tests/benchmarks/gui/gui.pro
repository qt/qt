TEMPLATE = subdirs
SUBDIRS = \
        animation \
        graphicsview \
        image \
        itemviews \
        kernel \
        math3d \
        painting \
        styles \
        text

TRUSTED_BENCHMARKS += \
    graphicsview/functional/GraphicsViewBenchmark \
    graphicsview/qgraphicsview

include(../trusted-benchmarks.pri)
