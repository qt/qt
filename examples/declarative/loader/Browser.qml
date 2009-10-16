import Qt 4.6

Rectangle {
    id: root
    property bool keyPressed: false
    width: parent.width
    height: parent.height
    color: palette.window
    FolderListModel {
        id: folders
        nameFilters: [ "*.qml" ]
        folder: "file:///E:/" // Documents on your S60 phone (or Windows E: drive)
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
            height: 48
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
                width: 46; height: 46
                Image { source: "images/fileopen.png"; anchors.centerIn: parent; visible: folders.isFolder(index)}
            }
            Text {
                id: nameText
                anchors.fill: parent; verticalAlignment: "AlignVCenter"
                text: fileName; anchors.leftMargin: 48
                font.pixelSize: 32
                color: wrapper.isCurrentItem ? palette.highlightedText : palette.text
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

    ListView {
        id: view
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: folders
        delegate: folderDelegate
        highlight: Rectangle { color: palette.highlight; visible: root.keyPressed }
        clip: true
        focus: true
        pressDelay: 100

        Keys.onPressed: {
            root.keyPressed = true;
            if (event.key == Qt.Key_Return || event.key == Qt.Key_Select || event.key == Qt.Key_Right) {
                view.currentItem.launch();
                event.accepted = true;
            }
        }
        Keys.onLeftPressed: folders.folder = up(folders.folder)
    }

    Rectangle {
        id: titleBar
        width: parent.width
        height: 48
        color: palette.lighter(palette.window); border.color: palette.mid

        Rectangle {
            id: upButton
            width: 48
            height: titleBar.height
            border.color: palette.mid; color: "transparent"

            Script {
                function up(path) {
                    var pos = path.toString().lastIndexOf("/");
                    return path.toString().substring(0, pos);
                }
            }

            Image { anchors.centerIn: parent; source: "images/up.png" }
            MouseRegion { id: upRegion; anchors.fill: parent
                onClicked: if (folders.parentFolder != "") folders.folder = folders.parentFolder
            }
            states: [
                State {
                    name: "pressed"
                    when: upRegion.pressed
                    PropertyChanges { target: upButton; color: palette.highlightedText }
                }
            ]
        }

        Text {
            anchors.left: upButton.right; anchors.right: parent.right; height: parent.height
            anchors.leftMargin: 4; anchors.rightMargin: 4
            text: folders.folder; color: palette.text
            elide: "ElideLeft"; horizontalAlignment: "AlignRight"; verticalAlignment: "AlignVCenter"
            font.pixelSize: 32
        }
    }
}
