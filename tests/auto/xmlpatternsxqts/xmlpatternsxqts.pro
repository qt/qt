TEMPLATE = subdirs
contains(QT_CONFIG,xmlpatterns) {
  SUBDIRS += lib
  test.depends = lib
}
SUBDIRS += test

# Needed on the win32-g++ setup and on the test machine arsia.
INCLUDEPATH += $$QT_BUILD_TREE/include/QtXmlPatterns/private \
               ../../../include/QtXmlPatterns/private

requires(contains(QT_CONFIG,private_tests))

