import Qt 4.6

Rectangle {
    id: root
    property bool keyPressed: false
    property variant folders: folders1
    property variant view: view1
    width: 320
    height: 480
    color: palette.window

    FolderListModel {
        id: folders1
        nameFilters: [ "*.qml" ]
        folder: qmlViewerFolder
    }
    FolderListModel {
        id: folders2
        nameFilters: [ "*.qml" ]
        folder: qmlViewerFolder
    }

    SystemPalette { id: palette }

    function down(path) {
        if (folders == folders1) {
            view = view2
            folders = folders2;
            view1.state = "exitLeft";
        } else {
            view = view1
            folders = folders1;
            view2.state = "exitLeft";
        }
        view.x = root.width;
        view.state = "current";
        view.focus = true;
        folders.folder = path;
    }
    function up() {
        var path = folders.parentFolder;
        if (folders == folders1) {
            view = view2
            folders = folders2;
            view1.state = "exitRight";
        } else {
            view = view1
            folders = folders1;
            view2.state = "exitRight";
        }
        view.x = -root.width;
        view.state = "current";
        view.focus = true;
        folders.folder = path;
    }

    Component {
        id: folderDelegate
        Rectangle {
            id: wrapper
            function launch() {
                if (folders.isFolder(index)) {
                    down(filePath);
                } else {
                    qmlViewer.launch(filePath);
                }
            }
            width: root.width
            height: 52
            color: "transparent"
            Rectangle {
                id: highlight; visible: false
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop { id: t1; position: 0.0; color: palette.highlight }
                    GradientStop { id: t2; position: 1.0; color: Qt.lighter(palette.highlight) }
                }
            }
            Item {
                width: 48; height: 48
                Image { source: "images/folder.png"; anchors.centerIn: parent; visible: folders.isFolder(index)}
            }
            Text {
                id: nameText
                anchors.fill: parent; verticalAlignment: Text.AlignVCenter
                text: fileName
                anchors.leftMargin: 54
                font.pixelSize: 32
                color: (wrapper.ListView.isCurrentItem && root.keyPressed) ? palette.highlightedText : palette.windowText
            }
            MouseArea {
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
        id: view1
        anchors.top: titleBar.bottom
        anchors.bottom: parent.bottom
        x: 0
        width: parent.width
        model: folders1
        delegate: folderDelegate
        highlight: Rectangle { color: palette.highlight; visible: root.keyPressed && view1.count != 0 }
        highlightMoveSpeed: 1000
        pressDelay: 100
        focus: true
        state: "current"
        states: [
            State {
                name: "current"
                PropertyChanges { target: view1; x: 0 }
            },
            State {
                name: "exitLeft"
                PropertyChanges { target: view1; x: -root.width }
            },
            State {
                name: "exitRight"
                PropertyChanges { target: view1; x: root.width }
            }
        ]
        transitions: [
            Transition {
                to: "current"
                SequentialAnimation {
                    NumberAnimation { properties: "x"; duration: 250 }
                }
            },
            Transition {
                NumberAnimation { properties: "x"; duration: 250 }
                NumberAnimation { properties: "x"; duration: 250 }
            }
        ]
        Keys.onPressed: { root.keyPressed = true; }
    }

    ListView {
        id: view2
        anchors.top: titleBar.bottom
        anchors.bottom: parent.bottom
        x: parent.width
        width: parent.width
        model: folders2
        delegate: folderDelegate
        highlight: Rectangle { color: palette.highlight; visible: root.keyPressed && view2.count != 0 }
        highlightMoveSpeed: 1000
        pressDelay: 100
        states: [
            State {
                name: "current"
                PropertyChanges { target: view2; x: 0 }
            },
            State {
                name: "exitLeft"
                PropertyChanges { target: view2; x: -root.width }
            },
            State {
                name: "exitRight"
                PropertyChanges { target: view2; x: root.width }
            }
        ]
        transitions: [
            Transition {
                to: "current"
                SequentialAnimation {
                    NumberAnimation { properties: "x"; duration: 250 }
                }
            },
            Transition {
                NumberAnimation { properties: "x"; duration: 250 }
            }
        ]
        Keys.onPressed: { root.keyPressed = true; }
    }

    Keys.onPressed: {
        root.keyPressed = true;
        if (event.key == Qt.Key_Return || event.key == Qt.Key_Select || event.key == Qt.Key_Right) {
            view.currentItem.launch();
            event.accepted = true;
        } else if (event.key == Qt.Key_Left) {
            up();
        }
    }

    BorderImage {
        source: "images/titlebar.sci";
        width: parent.width;
        height: 52
        y: -7
        id: titleBar

        Rectangle {
            id: upButton
            width: 48
            height: titleBar.height - 7
            color: "transparent"

            Image { anchors.centerIn: parent; source: "images/up.png" }
            MouseArea { id: upRegion; anchors.centerIn: parent
                width: 56
                height: 56
                onClicked: if (folders.parentFolder != "") up()
            }
            states: [
                State {
                    name: "pressed"
                    when: upRegion.pressed
                    PropertyChanges { target: upButton; color: palette.highlight }
                }
            ]
        }
        Rectangle {
            color: "gray"
            x: 48
            width: 1
            height: 44
        }

        Text {
            anchors.left: upButton.right; anchors.right: parent.right; height: parent.height
            anchors.leftMargin: 4; anchors.rightMargin: 4
            text: folders.folder
            color: "white"
            elide: Text.ElideLeft; horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter
            font.pixelSize: 32
        }
    }
}
