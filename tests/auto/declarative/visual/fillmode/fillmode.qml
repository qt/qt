import Qt 4.6

Rectangle {
    id: screen; width: 750; height: 600; color: "gray"
    property string source: "face.png"

    Grid {
        columns: 3
        Image { width: 250; height: 300; source: screen.source; fillMode: Image.Stretch }
        Image { width: 250; height: 300; source: screen.source; fillMode: Image.PreserveAspectFit; smooth: true }
        Image { width: 250; height: 300; source: screen.source; fillMode: Image.PreserveAspectCrop }
        Image { width: 250; height: 300; source: screen.source; fillMode: Image.Tile; smooth: true }
        Image { width: 250; height: 300; source: screen.source; fillMode: Image.TileHorizontally }
        Image { width: 250; height: 300; source: screen.source; fillMode: Image.TileVertically }
    }
}
