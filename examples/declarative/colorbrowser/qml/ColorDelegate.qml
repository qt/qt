import Qt 4.6

Package {
    Item { id: stack; Package.name: 'stack'; width: 110; height: 110; z: stack.PathView.z }
    Item { id: list; Package.name: 'list'; width: mainWindow.width + 4; height: 110 }
    Item { id: grid; Package.name: 'grid'; width: 110; height: 110 }

    Item {
        id: delegate

        property double randomAngle: Math.random() * (2 * 8 + 1) - 8
        property bool open: false

        width: 110; height: 110; z: stack.PathView.z

        Image { x: -6; y: -5; source: 'box-shadow.png'; smooth: true;  }
        Rectangle { color: hex; anchors.fill: parent; smooth: true }
        Image { id: box; source: 'box.png'; smooth: true; anchors.fill: parent }

        Binding {
            target: infoColorName; property: 'text'
            value: name; when: delegate.open && list.ListView.isCurrentItem
        }

        Binding {
            target: infoColorHex; property: 'text'
            value: hex; when: delegate.open && list.ListView.isCurrentItem
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton | Qt.LeftButton
            onClicked: {
                if (wrapper.state == 'inGrid' && mouse.button == Qt.RightButton) {
                    wrapper.state = ''
                } else if (wrapper.state == 'inGrid') {
                    grid.GridView.view.currentIndex = index;
                    wrapper.state = 'fullscreen'
                } else {
                    grid.GridView.view.currentIndex = index;
                    wrapper.state = 'inGrid'
                }
            }
        }

        states: [
        State {
            name: 'stacked'; when: wrapper.state == ''
            ParentChange { target: delegate; parent: stack; x: 0; y: 0; rotation: delegate.randomAngle }
            PropertyChanges { target: delegate; visible: stack.PathView.onPath ? 1.0 : 0.0 }
        },
        State {
            name: 'inGrid'; when: wrapper.state == 'inGrid'
            ParentChange { target: delegate; parent: grid; x: 24; y: 24 }
            PropertyChanges { target: delegate; open: true }
        },
        State {
            name: 'fullscreen'; when: wrapper.state == 'fullscreen'
            ParentChange { target: delegate; parent: list; x: 0; y: 0; width: mainWindow.width; height: mainWindow.height }
            PropertyChanges { target: box; opacity: 0 }
            PropertyChanges { target: delegate; open: true }
        }
        ]

        transitions: [
        Transition {
            from: 'stacked'; to: 'inGrid'
            SequentialAnimation {
                PauseAnimation { duration: 20 * index }
                ParentAnimation {
                    target: delegate; via: foreground
                    NumberAnimation { targets: delegate; properties: 'x,y,width,height,rotation'; duration: 600; easing.type: 'OutBack' }
                }
            }
        },
        Transition {
            from: 'inGrid'; to: 'stacked'
            ParentAnimation {
                target: delegate; via: foreground
                NumberAnimation { properties: 'x,y,width,height,rotation'; duration: 300; easing.type: 'InOutQuad' }
            }
        },
        Transition {
            from: 'inGrid'; to: 'fullscreen'
            SequentialAnimation {
                PauseAnimation { duration: grid.GridView.isCurrentItem ? 0 : 300 }
                ParentAnimation {
                    target: delegate; via: foreground
                    NumberAnimation {
                        properties: 'x,y,width,height,opacity'
                        duration: grid.GridView.isCurrentItem ? 300 : 1; easing.type: 'InOutQuad'
                    }
                }
            }
        },
        Transition {
            from: 'fullscreen'; to: 'inGrid'
            SequentialAnimation {
                PauseAnimation { duration: grid.GridView.isCurrentItem ? 3 : 0 }
                ParallelAnimation {
                    ParentAnimation {
                        target: delegate; via: foreground
                        NumberAnimation {
                            properties: 'x,y,width,height'
                            duration: grid.GridView.isCurrentItem ? 300 : 1; easing.type: 'InOutQuad'
                        }
                    }
                    NumberAnimation { properties: 'opacity'; duration: grid.GridView.isCurrentItem ? 300 : 1; easing.type: 'InOutQuad' }
                }
            }
        }
        ]
    }
}
