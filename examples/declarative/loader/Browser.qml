import Qt 4.6

Rect {
    id: Root
    width: parent.width
    height: parent.height
    color: activePalette.base
    FolderListModel {
        id: folders
        nameFilters: [ "*.qml" ]
//        folder: "E:"
    }

    Palette { id: activePalette; colorGroup: "Active" }

    Component {
        id: FolderDelegate
        Rect {
            id: Wrapper
            width: Root.width
            height: 32
            color: activePalette.base
            Rect {
                id: Highlight; visible: false
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop { id: t1; position: 0.0; color: activePalette.highlight }
                    GradientStop { id: t2; position: 1.0; color: activePalette.lighter(activePalette.highlight) }
                }
            }
            Item {
                width: 32; height: 32
                Image { source: "images/fileopen.png"; anchors.centerIn: parent; visible: folders.isFolder(index)}
            }
            Text {
                id: NameText
                anchors.fill: parent; vAlign: "AlignVCenter"
                text: fileName; anchors.leftMargin: 32
                font.pointSize: 10
                color: activePalette.windowText
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
                    SetProperties { target: Highlight; visible: true }
                    SetProperties { target: NameText; color: activePalette.highlightedText }
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

    ListView {
        anchors.top: TitleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: folders
        delegate: FolderDelegate
        clip: true
    }

    Rect {
        id: TitleBar
        width: parent.width
        height: 32
        color: activePalette.button; border.color: activePalette.mid

        Rect {
            id: UpButton
            width: 30
            height: TitleBar.height
            border.color: activePalette.mid; color: "transparent"
            MouseRegion { anchors.fill: parent; onClicked: folders.folder = up(folders.folder) }
            Image { anchors.centerIn: parent; source: "images/up.png" }
        }

        Text {
            anchors.left: UpButton.right; anchors.right: parent.right; height: parent.height
            anchors.leftMargin: 4; anchors.rightMargin: 4
            text: folders.folder; color: activePalette.buttonText
            elide: "ElideLeft"; hAlign: "AlignRight"; vAlign: "AlignVCenter"
        }
    }
}
