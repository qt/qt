qmlcalculator_src = $$PWD/../../declarative/calculator
symbian {
    load(data_caging_paths)
    qmlcalculator_uid3 = A000E3FB
    qmlcalculator_files.path = $$APP_PRIVATE_DIR_BASE/$$qmlcalculator_uid3
}
qmlcalculator_files.files = \
    $$qmlcalculator_src/qml/calculator/calculator.qml \
    $$qmlcalculator_src/qml/calculator/CalculatorCore
DEPLOYMENT += qmlcalculator_files
