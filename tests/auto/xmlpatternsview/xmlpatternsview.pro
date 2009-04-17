TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = ../xmlpatternsxqts test

contains(QT_CONFIG,xmlpatterns) {
  SUBDIRS += view
}
