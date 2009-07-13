TEMPLATE = subdirs
SUBDIRS = test
contains(QT_CONFIG,xmlpatterns) {
  SUBDIRS += lib
  !wince*:lib.file = lib/lib.pro
  test.depends = lib
}

# Needed on the win32-g++ setup and on the test machine arsia.
INCLUDEPATH += $$QT_BUILD_TREE/include/QtXmlPatterns/private \
               ../../../include/QtXmlPatterns/private

requires(contains(QT_CONFIG,private_tests))

