import Qt 4.6
import 'qml'

Rectangle {
    id: mainWindow
    width: 800; height: 480; color: '#454545'

    VisualDataModel { id: colorsVisualModel; delegate: colorsDelegate; model: ColorsModel }

    Component {
        id: colorsDelegate
        Package {

            Item {
                Package.name: 'grid'
                GridView {
                    id: gridView; model: visualModel.parts.grid; width: mainWindow.width; height: mainWindow.height
                    cellWidth: 160; cellHeight: 160; interactive: false
                    onCurrentIndexChanged: listView.positionViewAtIndex(currentIndex, ListView.Contain)
                }
            }

            Item {
                Package.name: 'fullscreen'
                ListView {
                    id: listView; model: visualModel.parts.list; orientation: Qt.Horizontal
                    width: mainWindow.width; height: mainWindow.height; interactive: false
                    onCurrentIndexChanged: gridView.positionViewAtIndex(currentIndex, GridView.Contain)
                    highlightRangeMode: ListView.StrictlyEnforceRange; snapMode: ListView.SnapOneItem
                }
            }

            Item {
                Package.name: 'stack'
                id: wrapper
                width: 260; height: 240

                VisualDataModel { id: visualModel; model: colors; delegate: ColorDelegate {} }

                PathView {
                    id: pathView; model: visualModel.parts.stack; anchors.centerIn: parent
                    pathItemCount: 3
                    path: Path {
                        PathAttribute { name: 'z'; value: 9999.0 }
                        PathLine { x: 1; y: 1 }
                        PathAttribute { name: 'z'; value: 0.0 }
                    }
                }

                Item {
                    anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 20
                    width: albumTitle.width + 20 ; height: albumTitle.height + 4
                    Text { id: albumTitle; text: name; color: 'white'; font.bold: true; anchors.centerIn: parent }
                }

                MouseArea { anchors.fill: parent; onClicked: wrapper.state = 'inGrid' }

                states: [
                State {
                    name: 'inGrid'
                    PropertyChanges { target: gridView; interactive: true }
                    PropertyChanges { target: shade; opacity: 1 }
                },
                State {
                    name: 'fullscreen'; extend: 'inGrid'
                    PropertyChanges { target: gridView; interactive: false }
                    PropertyChanges { target: listView; interactive: true }
                    PropertyChanges { target: infobox; opacity: 1 }
                }
                ]

                transitions: [
                    Transition {
                        from: ''; to: 'inGrid'; reversible: true
                        NumberAnimation { target: shade; properties: 'opacity'; duration: 300 }
                    },
                    Transition {
                        from: 'inGrid'; to: 'fullscreen'; reversible: true
                        SequentialAnimation {
                            PauseAnimation { duration: 300 }
                            NumberAnimation { target: infobox; properties: 'opacity'; duration: 300 }
                        }
                    }
                ]
            }
        }
    }

    GridView { width: parent.width; height: parent.height; cellWidth: 260; cellHeight: 240; model: colorsVisualModel.parts.stack }
    Rectangle { id: shade; color: '#454545'; width: parent.width; height: parent.height; opacity: 0 }
    ListView { anchors.fill: parent; model: colorsVisualModel.parts.grid; interactive: false }
    ListView { anchors.fill: parent; model: colorsVisualModel.parts.fullscreen; interactive: false }
    Item { id: foreground; anchors.fill: parent }

    Column {
        id: infobox; opacity: 0
        anchors { left: parent.left; leftMargin: 20; bottom: parent.bottom; bottomMargin: 20 }
        Text { id: infoColorName; color: '#eeeeee'; style: Text.Outline; styleColor: '#222222'; font.pointSize: 18 }
        Text { id: infoColorHex; color: '#eeeeee'; style: Text.Outline; styleColor: '#222222'; font.pointSize: 14 }
    }
}
