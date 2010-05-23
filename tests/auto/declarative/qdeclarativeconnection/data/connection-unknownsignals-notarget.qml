import Qt 4.7

Item {
    id: screen

    Connections { objectName: "connections"; target: null; onNotFooBar: {} }
}
