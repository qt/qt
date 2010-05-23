import Qt 4.7
Rectangle {
    id: myRectangle
    property color sourceColor: "blue"
    width: 100; height: 100
    color: "red"
    states: State {
        name: "blue"
        PropertyChanges {
            objectName: "changes"
            target: myRectangle; explicit: true
            color: sourceColor
        }
    }
}
