TEMPLATE=app
CONFIG -= qt separate_debug_info
LIBS =
QT =
QMAKE_LINK = @: dummy

sources.files = \
    CalcButton.qml \
    calculator.js \
    calculator.qml
sources.path = $$[QT_INSTALL_DEMOS]/declarative/calculator
INSTALLS = sources

