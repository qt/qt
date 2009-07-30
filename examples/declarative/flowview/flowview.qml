import Qt 4.6

Rect {
    width: 800
    height: 800
    color: "black"

    Rect {
        id: MyPhone
        transformOrigin: "Center"
        anchors.centeredIn: parent
        width: 800
        height: 480
        clip: true

        states: State { 
            name: "rotated"
            SetProperties { target: ListView; z: 2 }
            SetProperties { target: TopBar; y: -30 }
            SetProperties { target: BottomBar; y: 480 }
            SetProperties { target: LeftBar; x: 0 }
            SetProperties { target: RightBar; x: 770 }
        }
        transitions: Transition {
            fromState: "" ; toState: "rotated"
            reversible: true
            SequentialAnimation {
                NumberAnimation { filter: [TopBar, BottomBar]; properties: "x,y"; easing: "easeInOutQuad" }
                NumberAnimation { filter: [LeftBar, RightBar]; properties: "x,y"; easing: "easeInOutQuad"}
            }
        }

        color: "lightsteelblue"

        VisualModel {
            id: Model
            model: ListModel {
                ListElement { background: "red"; weblet: "rect.qml"  }
                ListElement { background: "yellow"; weblet: "rect.qml"  }
                ListElement { background: "blue"; weblet: "rect.qml"  }
                ListElement { background: "green"; weblet: "flickr.qml"  }
                ListElement { background: "orange"; weblet: "rect.qml"  }
                ListElement { background: "lightblue"; weblet: "rect.qml"  }
            }
            delegate: Package {
                Item { id: List; Package.name: "list"; width:120; height: 400; }
                Item { id: Grid; Package.name: "grid"; width:400; height: 120; }
                Loader { id: MyContent; width:400; height: 120; source: weblet }

                StateGroup {
                    states: [
                        State { 
                            name: "InList"
                            when: MyPhone.state == "rotated"
                            ParentChange { target: MyContent; parent: List }
                            SetProperties { target: MyContent; x: 120; y: 0; rotation: 90}
                        },
                        State { 
                            name: "InGrid"
                            when: MyPhone.state != "rotated"
                            ParentChange { target: MyContent; parent: Grid }
                            SetProperties { target: MyContent; x: 0; y: 0; }
                        }
                    ]
                    transitions: [
                    Transition {
                        fromState: "*"; toState: "InGrid"
                        SequentialAnimation { 
                            ParentChangeAction{} 
                            PauseAnimation { duration: 50 * List.FlowView.column }
                            NumberAnimation { properties: "x,y,rotation"; easing: "easeInOutQuad" } 
                        } 
                    },
                    Transition {
                        fromState: "*"; toState: "InList"
                        SequentialAnimation { 
                            ParentChangeAction{} 
                            PauseAnimation { duration: 50 * (Grid.FlowView.row * 2 + Grid.FlowView.column) }
                            NumberAnimation { properties: "x,y,rotation"; easing: "easeInOutQuad" } 
                        } 
                    }
                    ]
                }

            }
        }

        Item {
            FlowView {
                id: ListView
                vertical: true
                y: 40
                x: 40
                width: 800
                height: 400
                column: 1
                model: Model.parts.list
            }

            FlowView {
                z: 1
                y: 60
                width: 800
                height: 400
                column: 2
                model: Model.parts.grid
            }
        }

        Rect {
            id: TopBar
            width: 800
            height: 30
        }
        Rect {
            id: BottomBar
            width: 800
            height: 30
            y: 450
        }
        Rect {
            id: LeftBar
            x: -30
            width: 30
            height: 480
        }
        Rect {
            id: RightBar
            x: 800
            width: 30
            height: 480
        }
    }

    Rect {
        width: 80
        height: 80
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        Text { text: "Switch" }
        MouseRegion {
            anchors.fill: parent
            onClicked: if(MyPhone.state == "rotated") MyPhone.state=""; else MyPhone.state = "rotated";
        }
    }

}
