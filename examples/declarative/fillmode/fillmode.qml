import Qt 4.6

Image {
    width: 400
    height: 250
    source: "face.png"
    fillMode: SequentialAnimation {
        running: true
        repeat: true
        PropertyAction { value: "Stretch" }
        PropertyAction { target: Label; property: "text"; value: "Stretch" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: "PreserveAspectFit" }
        PropertyAction { target: Label; property: "text"; value: "PreserveAspectFit" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: "PreserveAspectCrop" }
        PropertyAction { target: Label; property: "text"; value: "PreserveAspectCrop" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: "Tile" }
        PropertyAction { target: Label; property: "text"; value: "Tile" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: "TileHorizontally" }
        PropertyAction { target: Label; property: "text"; value: "TileHorizontally" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: "TileVertically" }
        PropertyAction { target: Label; property: "text"; value: "TileVertically" }
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
