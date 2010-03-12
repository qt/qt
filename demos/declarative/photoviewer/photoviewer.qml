import Qt 4.6
import PhotoViewerCore 1.0

Image {
    id: mainWindow

    property real downloadProgress: 0
    property bool imageLoading: false

    width: 800; height: 480; source: "PhotoViewerCore/images/background.png"; fillMode: Image.Tile

    Script { source: "PhotoViewerCore/script/script.js" }

    ListModel {
        id: photosModel
        ListElement { tag: "Flowers" }
        ListElement { tag: "Savanna" }
        ListElement { tag: "Central Park" }
    }

    VisualDataModel {
        id: albumVisualModel; delegate: AlbumDelegate { }
        model: photosModel
    }

    GridView { width: parent.width; height: parent.height; cellWidth: 210; cellHeight: 220; model: albumVisualModel.parts.album }

    Image {
        id: albumsShade; source: "PhotoViewerCore/images/background.png"; fillMode: Image.Tile
        width: parent.width; height: parent.height; opacity: 0
    }

    ListView { anchors.fill: parent; model: albumVisualModel.parts.browser; interactive: false }

    Tag { id: backTag; label: "Back"; rotation: 3; x: parent.width - backTag.width - 6; y: -backTag.height - 8 }

    Rectangle { id: photosShade; color: 'black'; width: parent.width; height: parent.height; opacity: 0 }
    ListView { anchors.fill: parent; model: albumVisualModel.parts.fullscreen; interactive: false }
    Item { id: foreground; anchors.fill: parent }

    ProgressBar {
        progress: mainWindow.downloadProgress; width: parent.width; height: 4
        anchors.bottom: parent.bottom; opacity: mainWindow.imageLoading
    }
}
