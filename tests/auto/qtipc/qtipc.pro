TEMPLATE=subdirs
SUBDIRS=\
    lackey \
    qsharedmemory \
    qsystemsemaphore \

!contains(QT_CONFIG, private_tests): SUBDIRS -= \
    lackey \
    qsharedmemory \
    qsystemsemaphore \
