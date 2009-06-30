Rect {
    id: Root
    width: 300
    height: 400
    FolderListModel {
        id: folders
        nameFilters: [ "*.qml" ]
    }

    Component {
        id: FolderDelegate
        Text {
            id: Wrapper
            width: Root.width
            text: fileName
            font.bold: true
            font.size: 14
            MouseRegion {
                anchors.fill: parent
                onClicked: {
                    if (folders.isFolder(index)) {
                        folders.folder = filePath;
                    } else {
                        Root.qml = filePath;
                    }
                }
            }
        }
    }

    Text { id: DirText; text: folders.folder }

    ListView {
        anchors.top: DirText.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: folders
        delegate: FolderDelegate
    }
}
