import Qt 4.6

Item {
    id: screen; width: 50

    property bool tested: false
    signal testMe

    Connection { sender: screen; signal: "widthChanged()"; script: screen.tested = true }
}
