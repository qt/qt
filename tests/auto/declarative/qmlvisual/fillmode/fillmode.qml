import QtQuick 1.0

/*
    This is a static display test of the various Image fill modes. See the png file in the data
    subdirectory to see what the image should look like.
*/

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
