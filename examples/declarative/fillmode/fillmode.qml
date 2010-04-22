import Qt 4.7
import "content"

Rectangle {
    id: window

    width: 800; height: 480
    color: "#cdcdcd"

    Grid {
        id: grid
        anchors { fill: parent; topMargin: 10; leftMargin: 10; rightMargin: 10; bottomMargin: 10 }
        columns: 3; rows: 2; spacing: 20

        QtLogo { fillMode: Image.Stretch; label: "Stretch" }
        QtLogo { fillMode: Image.PreserveAspectFit; label: "PreserveAspectFit" }
        QtLogo { fillMode: Image.PreserveAspectCrop; label: "PreserveAspectCrop" }
        QtLogo { fillMode: Image.Tile; label: "Tile" }
        QtLogo { fillMode: Image.TileHorizontally; label: "TileHorizontally" }
        QtLogo { fillMode: Image.TileVertically; label: "TileVertically" }
    }
}
