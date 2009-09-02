import Qt 4.6
Rectangle {
    id: MyRectangle

    property color sourceColor: "red"
    property color sourceColor2: "blue"
    width: 100; height: 100
    color: sourceColor
    states: State {
        name: "blue"
        PropertyChanges { target: MyRectangle; color: sourceColor2 }
    }
}