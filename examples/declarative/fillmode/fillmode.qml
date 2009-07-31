import Qt 4.6

Image {
    width: 400
    height: 250
    source: "face.png"
    fillMode: SequentialAnimation {
        running: true
        repeat: true
        SetPropertyAction { value: "Stretch" }
        SetPropertyAction { target: Label; property: "text"; value: "Stretch" }
        PauseAnimation { duration: 1000 }
        SetPropertyAction { value: "PreserveAspect" }
        SetPropertyAction { target: Label; property: "text"; value: "PreserveAspect" }
        PauseAnimation { duration: 1000 }
        SetPropertyAction { value: "Tile" }
        SetPropertyAction { target: Label; property: "text"; value: "Tile" }
        PauseAnimation { duration: 1000 }
        SetPropertyAction { value: "TileHorizontally" }
        SetPropertyAction { target: Label; property: "text"; value: "TileHorizontally" }
        PauseAnimation { duration: 1000 }
        SetPropertyAction { value: "TileVertically" }
        SetPropertyAction { target: Label; property: "text"; value: "TileVertically" }
        PauseAnimation { duration: 1000 }
    }
    Text {
        id: Label
        font.size: 24
        color: "blue"
        style: "Outline"
        styleColor: "white"
        anchors { centeredIn: parent }
    }
    Rect {
        pen.color: "black"
        color: "transparent"
        anchors { fill: parent; rightMargin: 1; bottomMargin: 1}
    }
}
