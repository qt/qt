import Qt 4.6

Rectangle {
    width: 100; height: 100; color: "red"
    signal somethingHappened
    onSomethingHappened: QtObject {}
}
