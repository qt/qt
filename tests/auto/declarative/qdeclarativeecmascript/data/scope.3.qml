import Qt 4.7

Item {
    id: root

    property int foo: 12
    property int console: 11

    property bool test1: foo == 12
    property bool test2: console != 11
    property bool test3: root.console == 11
}

