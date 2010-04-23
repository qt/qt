TEMPLATE=subdirs
SUBDIRS=\
           headers \
           bic \
           compiler \
           compilerwarnings \
           linguist \
           maketestselftest \
           moc \
           uic \
           guiapplauncher \
           qmake \
           rcc \
           symbols \
           #atwrapper \     # These tests need significant updating,
           #uiloader \      # they have hardcoded machine names etc.

contains(QT_CONFIG,qt3support):SUBDIRS+=uic3

