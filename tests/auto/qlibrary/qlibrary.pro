QT -= gui
TEMPLATE    =	subdirs
CONFIG  += ordered

symbian: {
# Can't build two versions of lib with same name in symbian, so just build one
SUBDIRS =   lib2 \
            tst
} else:integrity {
# no shared support, empty test
} else {
SUBDIRS =   lib \
            lib2 \
            tst
}
TARGET = tst_qlibrary

# no special install rule for subdir
INSTALLS =
CONFIG += parallel_test
