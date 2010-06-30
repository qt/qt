qmlcalculator_src = $$PWD/../../declarative/calculator
symbian {
    qmlcalculator_uid3 = A000E3FB
    qmlcalculator_files.path = $$APP_PRIVATE_DIR_BASE/$$qmlcalculator_uid3
}
qmlcalculator_files.sources = $$qmlcalculator_src/calculator.qml $$qmlcalculator_src/Core
DEPLOYMENT += qmlcalculator_files
