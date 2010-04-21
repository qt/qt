import Qt 4.7

Image {
    width: 400
    height: 250
    source: "face.png"

    SequentialAnimation on fillMode {
        loops: Animation.Infinite
        PropertyAction { value: Image.Stretch }
        PropertyAction { target: label; property: "text"; value: "Stretch" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: Image.PreserveAspectFit }
        PropertyAction { target: label; property: "text"; value: "PreserveAspectFit" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: Image.PreserveAspectCrop }
        PropertyAction { target: label; property: "text"; value: "PreserveAspectCrop" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: Image.Tile }
        PropertyAction { target: label; property: "text"; value: "Tile" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: Image.TileHorizontally }
        PropertyAction { target: label; property: "text"; value: "TileHorizontally" }
        PauseAnimation { duration: 1000 }
        PropertyAction { value: Image.TileVertically }
        PropertyAction { target: label; property: "text"; value: "TileVertically" }
        PauseAnimation { duration: 1000 }
    }

    Text {
        id: label
        font.pointSize: 24
        color: "blue"
        style: Text.Outline
        styleColor: "white"
        anchors.centerIn: parent
    }

    Rectangle {
        border.color: "black"
        color: "transparent"
        anchors { fill: parent; rightMargin: 1; bottomMargin: 1 }
    }
}
