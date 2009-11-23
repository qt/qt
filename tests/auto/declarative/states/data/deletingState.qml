import Qt 4.6
Rectangle {
    id: MyRectangle
    width: 100; height: 100
    color: "red"
    StateGroup {
        id: stateGroup
        states: State {
            name: "blue"
            PropertyChanges { target: MyRectangle; color: "blue" }
        }
    }
}
