Rect {
    id: Root
    width: parent.width
    height: parent.height
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
                        qmlLauncher.launch(filePath);
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
