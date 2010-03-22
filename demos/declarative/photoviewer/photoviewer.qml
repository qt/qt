import Qt 4.6
import PhotoViewerCore 1.0

Rectangle {
    id: mainWindow

    property real downloadProgress: 0
    property bool imageLoading: false
    property bool editMode: false

    width: 800; height: 480; color: "#d5d6d8"

    ListModel {
        id: photosModel
        ListElement { tag: "Flowers" }
        ListElement { tag: "Savanna" }
        ListElement { tag: "Central Park" }
    }

    VisualDataModel { id: albumVisualModel; model: photosModel; delegate: AlbumDelegate {} }

    GridView {
        id: albumView; width: parent.width; height: parent.height; cellWidth: 210; cellHeight: 220
        model: albumVisualModel.parts.album; visible: albumsShade.opacity != 1.0
    }

    Column {
        spacing: 20; anchors { bottom: parent.bottom; right: parent.right; rightMargin: 20; bottomMargin: 20 }
        Button { id: deleteButton; label: "Edit"; rotation: -2; onClicked: mainWindow.editMode = !mainWindow.editMode }
        Button {
            id: newButton; label: "New"; rotation: 3
            onClicked: {
                photosModel.append( { tag: "" } )
                albumView.positionViewAtIndex(albumView.count - 1, GridView.Contain)
            }
        }
    }

    Rectangle {
        id: albumsShade; color: mainWindow.color
        width: parent.width; height: parent.height; opacity: 0.0
    }

    ListView { anchors.fill: parent; model: albumVisualModel.parts.browser; interactive: false }

    Button { id: backButton; label: "Back"; rotation: 3; x: parent.width - backButton.width - 6; y: -backButton.height - 8 }

    Rectangle { id: photosShade; color: 'black'; width: parent.width; height: parent.height; opacity: 0; visible: opacity != 0.0 }

    ListView { anchors.fill: parent; model: albumVisualModel.parts.fullscreen; interactive: false }

    Item { id: foreground; anchors.fill: parent }

    ProgressBar {
        progress: mainWindow.downloadProgress; width: parent.width; height: 4
        anchors.bottom: parent.bottom; opacity: mainWindow.imageLoading; visible: opacity != 0.0
    }
}
