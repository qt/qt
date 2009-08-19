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
        SetPropertyAction { value: "PreserveAspectFit" }
        SetPropertyAction { target: Label; property: "text"; value: "PreserveAspectFit" }
        PauseAnimation { duration: 1000 }
        SetPropertyAction { value: "PreserveAspectCrop" }
        SetPropertyAction { target: Label; property: "text"; value: "PreserveAspectCrop" }
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
        font.pointSize: 24
        color: "blue"
        style: "Outline"
        styleColor: "white"
        anchors { centerIn: parent }
    }
    Rectangle {
        border.color: "black"
        color: "transparent"
        anchors { fill: parent; rightMargin: 1; bottomMargin: 1}
    }
}
