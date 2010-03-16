import Qt 4.6

Component {
    id: albumDelegate
    Package {

        Item {
            Package.name: 'browser'
            GridView {
                id: photosGridView; model: visualModel.parts.grid; width: mainWindow.width; height: mainWindow.height - 21
                x: 0; y: 21; cellWidth: 160; cellHeight: 153; interactive: false
                onCurrentIndexChanged: photosListView.positionViewAtIndex(currentIndex, ListView.Contain)
            }
        }

        Item {
            Package.name: 'fullscreen'
            ListView {
                id: photosListView; model: visualModel.parts.list; orientation: Qt.Horizontal
                width: mainWindow.width; height: mainWindow.height; interactive: false
                onCurrentIndexChanged: photosGridView.positionViewAtIndex(currentIndex, GridView.Contain)
                highlightRangeMode: ListView.StrictlyEnforceRange; snapMode: ListView.SnapOneItem
            }
        }

        Item {
            Package.name: 'album'
            id: albumWrapper
            width: 210; height: 220

            VisualDataModel {
                id: visualModel; delegate: PhotoDelegate { }
                model: RssModel { id: rssModel; tags: tag }
            }

            BusyIndicator {
                anchors { centerIn: parent; verticalCenterOffset: -20 }
                on: rssModel.status != XmlListModel.Ready
            }

            PathView {
                id: photosPathView; model: visualModel.parts.stack
                anchors.centerIn: parent; anchors.verticalCenterOffset: -20
                pathItemCount: 5
                path: Path {
                    PathAttribute { name: 'z'; value: 9999.0 }
                    PathLine { x: 1; y: 1 }
                    PathAttribute { name: 'z'; value: 0.0 }
                }
            }

            Tag {
                anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom
                label: tag; rotation: Math.random() * (2 * 6 + 1) - 6
            }

            MouseArea { anchors.fill: parent; onClicked: albumWrapper.state = 'inGrid' }

            states: [
            State {
                name: 'inGrid'
                PropertyChanges { target: photosGridView; interactive: true }
                PropertyChanges { target: albumsShade; opacity: 1 }
                PropertyChanges{ target: backTag; onClicked: albumWrapper.state = ''; y: 6 }
            },
            State {
                name: 'fullscreen'; extend: 'inGrid'
                PropertyChanges { target: photosGridView; interactive: false }
                PropertyChanges { target: photosListView; interactive: true }
                PropertyChanges { target: photosShade; opacity: 1 }
                PropertyChanges{ target: backTag; y: -backTag.height - 8 }
            }
            ]

            transitions: [
            Transition {
                from: '*'; to: 'inGrid'
                SequentialAnimation {
                    NumberAnimation { properties: 'opacity'; duration: 250 }
                    PauseAnimation { duration: 350 }
                    NumberAnimation { target: backTag; properties: "y"; duration: 200; easing.type: "OutQuad" }
                }
            },
            Transition {
                from: 'inGrid'; to: '*'
                NumberAnimation { properties: "y,opacity"; easing.type: "OutQuad"; duration: 300 }
            }
            ]
        }
    }
}
