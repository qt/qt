qmlclocks_src = $$PWD/../../../examples/declarative/toys/clocks
symbian {
    qmlclocks_uid3 = E19225B9
    qmlclocks_files.path = ../$$qmlclocks_uid3
}
qmlclocks_files.sources = $$qmlclocks_src/clocks.qml $$qmlclocks_src/content
DEPLOYMENT += qmlclocks_files
