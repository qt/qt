import Qt 4.6

Rectangle {
    width: 800
    height: 800
    color: "black"

    Rectangle {
        id: myPhone
        transformOrigin: "Center"
        anchors.centerIn: parent
        width: 800
        height: 480
        clip: true

        states: State {
            name: "rotated"
            PropertyChanges { target: myListView; z: 2 }
            PropertyChanges { target: topBar; y: -30 }
            PropertyChanges { target: bottomBar; y: 480 }
            PropertyChanges { target: leftBar; x: 0 }
            PropertyChanges { target: rightBar; x: 770 }
        }
        transitions: Transition {
            from: "" ; to: "rotated"
            reversible: true
            SequentialAnimation {
                NumberAnimation { targets: [topBar, bottomBar]; properties: "x,y"; easing: "easeInOutQuad" }
                NumberAnimation { targets: [leftBar, rightBar]; properties: "x,y"; easing: "easeInOutQuad"}
            }
        }

        color: "lightsteelblue"

        VisualDataModel {
            id: model
            model: ListModel {
                ListElement { background: "red"; weblet: "RoundedRect.qml"  }
                ListElement { background: "yellow"; weblet: "RoundedRect.qml"  }
                ListElement { background: "blue"; weblet: "RoundedRect.qml"  }
                ListElement { background: "green"; weblet: "FlickrView.qml"  }
                ListElement { background: "orange"; weblet: "RoundedRect.qml"  }
                ListElement { background: "lightblue"; weblet: "RoundedRect.qml"  }
            }
            delegate: Package {
                Item { id: list; Package.name: "list"; width:120; height: 400; }
                Item { id: gridItem; Package.name: "grid"; width:400; height: 120; }
                Loader { id: myContent; width:400; height: 120; source: weblet }

                StateGroup {
                    states: [
                        State {
                            name: "InList"
                            when: myPhone.state == "rotated"
                            ParentChange { target: myContent; parent: list }
                            PropertyChanges { target: myContent; x: 120; y: 0; rotation: 90}
                        },
                        State {
                            name: "InGrid"
                            when: myPhone.state != "rotated"
                            ParentChange { target: myContent; parent: gridItem }
                            PropertyChanges { target: myContent; x: 0; y: 0; }
                        }
                    ]
                    transitions: [
                        Transition {
                            from: "*"; to: "InGrid"
                            SequentialAnimation {
                                ParentAction{}
                                PauseAnimation { duration: 50 * list.FlowView.column }
                                NumberAnimation { properties: "x,y,rotation"; easing: "easeInOutQuad" }
                            }
                        },
                        Transition {
                            from: "*"; to: "InList"
                            SequentialAnimation {
                                ParentAction{}
                                PauseAnimation { duration: 50 * (gridItem.FlowView.row * 2 + gridItem.FlowView.column) }
                                NumberAnimation { properties: "x,y,rotation"; easing: "easeInOutQuad" }
                            }
                        }
                    ]
                }

            }
        }

        Item {
            FlowView {
                id: myListView
                vertical: true
                y: 40
                x: 40
                width: 800
                height: 400
                column: 1
                model: model.parts.list
            }

            FlowView {
                z: 1
                y: 60
                width: 800
                height: 400
                column: 2
                model: model.parts.grid
            }
        }

        Rectangle {
            id: topBar
            width: 800
            height: 30
        }
        Rectangle {
            id: bottomBar
            width: 800
            height: 30
            y: 450
        }
        Rectangle {
            id: leftBar
            x: -30
            width: 30
            height: 480
        }
        Rectangle {
            id: rightBar
            x: 800
            width: 30
            height: 480
        }
    }

    Rectangle {
        width: 80
        height: 80
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        Text { text: "Switch" }
        MouseRegion {
            anchors.fill: parent
            onClicked: if(myPhone.state == "rotated") myPhone.state=""; else myPhone.state = "rotated";
        }
    }

}
