qmlcalculator_src = $$PWD/../../declarative/calculator
symbian {
    qmlcalculator_uid3 = EA8EBD98
    qmlcalculator_files.path = ../$$qmlcalculator_uid3
}
qmlcalculator_files.sources = $$qmlcalculator_src/calculator.qml $$qmlcalculator_src/Core
DEPLOYMENT += qmlcalculator_files
