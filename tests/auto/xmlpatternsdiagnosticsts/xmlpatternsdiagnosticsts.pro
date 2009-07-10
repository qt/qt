TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = ../xmlpatternsxqts test
requires(contains(QT_CONFIG,private_tests))
