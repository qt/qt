import Qt 4.6

Rectangle {
    id: root
    width: parent.width
    height: parent.height
    color: palette.base
    FolderListModel {
        id: folders
        nameFilters: [ "*.qml" ]
//        folder: "E:"
    }

    SystemPalette { id: palette; colorGroup: Qt.Active }

    Component {
        id: folderDelegate
        Rectangle {
            id: wrapper
            function launch() {
                if (folders.isFolder(index)) {
                    folders.folder = filePath;
                } else {
                    qmlLauncher.launch(filePath);
                }
            }
            width: root.width
            height: 32
            color: "transparent"
            Rectangle {
                id: highlight; visible: false
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop { id: t1; position: 0.0; color: palette.highlight }
                    GradientStop { id: t2; position: 1.0; color: palette.lighter(palette.highlight) }
                }
            }
            Item {
                width: 32; height: 32
                Image { source: "images/fileopen.png"; anchors.centerIn: parent; visible: folders.isFolder(index)}
            }
            Text {
                id: nameText
                anchors.fill: parent; verticalAlignment: "AlignVCenter"
                text: fileName; anchors.leftMargin: 32
                font.pointSize: 10
                color: palette.windowText
            }
            MouseRegion {
                id: mouseRegion
                anchors.fill: parent
                onClicked: { launch() }
            }
            states: [
                State {
                    name: "pressed"
                    when: mouseRegion.pressed
                    PropertyChanges { target: highlight; visible: true }
                    PropertyChanges { target: nameText; color: palette.highlightedText }
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
        id: view
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: folders
        delegate: folderDelegate
        highlight: Rectangle { color: "#FFFBAF" }
        clip: true
        focus: true
        Keys.onPressed: {
            if (event.key == Qt.Key_Return || event.key == Qt.Key_Select) {
                view.currentItem.launch();
                event.accepted = true;
            }
        }
    }

    Rectangle {
        id: titleBar
        width: parent.width
        height: 32
        color: palette.button; border.color: palette.mid

        Rectangle {
            id: upButton
            width: 30
            height: titleBar.height
            border.color: palette.mid; color: "transparent"
            MouseRegion { anchors.fill: parent; onClicked: folders.folder = up(folders.folder) }
            Image { anchors.centerIn: parent; source: "images/up.png" }
        }

        Text {
            anchors.left: upButton.right; anchors.right: parent.right; height: parent.height
            anchors.leftMargin: 4; anchors.rightMargin: 4
            text: folders.folder; color: palette.buttonText
            elide: "ElideLeft"; horizontalAlignment: "AlignRight"; verticalAlignment: "AlignVCenter"
        }
    }
}
