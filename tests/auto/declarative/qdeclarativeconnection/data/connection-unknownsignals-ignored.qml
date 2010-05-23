import Qt 4.7

Item {
    id: screen

    Connections { target: screen; onNotFooBar1: {} ignoreUnknownSignals: true }
    Connections { objectName: "connections"; onNotFooBar2: {} ignoreUnknownSignals: true }
}
