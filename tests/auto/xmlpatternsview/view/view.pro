TARGET   = xmlpatternsview
TEMPLATE = app
DESTDIR  = $$QT_BUILD_TREE/bin

include (../../xmlpatterns.pri)

# We add gui here, since xmlpatterns.pri pull it out.
QT      += xmlpatterns xml gui

LIBS += -l$$XMLPATTERNS_SDK

HEADERS = FunctionSignaturesView.h  \
          MainWindow.h              \
          TestCaseView.h            \
          TestResultView.h          \
          TreeSortFilter.h          \
          UserTestCase.h            \
          XDTItemItem.h

SOURCES = FunctionSignaturesView.cpp    \
          main.cpp                      \
          MainWindow.cpp                \
          TestCaseView.cpp              \
          TestResultView.cpp            \
          TreeSortFilter.cpp            \
          UserTestCase.cpp              \
          XDTItemItem.cpp

FORMS   = ui_BaseLinePage.ui            \
          ui_MainWindow.ui              \
          ui_TestCaseView.ui            \
          ui_TestResultView.ui          \
          ui_FunctionSignaturesView.ui

INCLUDEPATH += $$QT_SOURCE_TREE/tests/auto/xmlpatternsxqts/lib/ $$QT_BUILD_TREE/include/QtXmlPatterns/private
