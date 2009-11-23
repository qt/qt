import Qt 4.6
Rectangle {
    id: MyRectangle
    width: 100; height: 100
    color: "red"
    states: State {
        name: "blue"
        PropertyChanges { target: MyRectangle; color: "blue"; objectName: "pc1" }
        PropertyChanges { target: MyRectangle; radius: 5; objectName: "pc2" }
    }
}
