Rect {
    id: Root
    width: parent.width
    height: parent.height
    FolderListModel {
        id: folders
        nameFilters: [ "*.qml" ]
//        folder: "E:"
    }

    Component {
        id: FolderDelegate
        Rect {
            id: Wrapper
            width: Root.width
            height: NameText.height
            Text {
                id: NameText
                text: fileName
                font.bold: true
                font.size: 12
            }
            MouseRegion {
                id: Mouse
                anchors.fill: parent
                onClicked: {
                    if (folders.isFolder(index)) {
                        folders.folder = filePath;
                    } else {
                        qmlLauncher.launch(filePath);
                    }
                }
            }
            states: [
                State {
                    name: "pressed"
                    when: Mouse.pressed
                    SetProperties { target: Wrapper; color: "#bbbbbb" }
                }
            ]
        }
    }

    Script {
        function up(path) {
            var pos = path.toString().lastIndexOf("/");
            return path.toString().substring(0, pos);
        }
    }

    Rect {
        id: UpButton
        width: 30
        height: UpText.height
        color: "grey"
        MouseRegion { anchors.fill: parent; onClicked: folders.folder = up(folders.folder) }
        Text { id: UpText; text: "Up" }
    }

    Text { anchors.left: UpButton.right; text: folders.folder }

    ListView {
        anchors.top: UpButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: folders
        delegate: FolderDelegate
        clip: true
    }
}
