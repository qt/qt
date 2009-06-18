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

        states: State { 
            name: "rotated"
            SetProperties { target: ListView; z: 2 }
        }

        color: "lightsteelblue"

        VisualModel {
            id: Model
            model: ListModel {
                ListElement { stuff: "red" }
                ListElement { stuff: "yellow" }
                ListElement { stuff: "blue" }
                ListElement { stuff: "green" }
                ListElement { stuff: "orange" }
                ListElement { stuff: "lightblue" }
            }
            delegate: Package {
                Item { id: List; Package.name: "list"; width:120; height: 400; }
                Item { id: Grid; Package.name: "grid"; width:400; height: 133; }
                Rect { id: MyContent; width:400; height: 133; color: stuff; }

                StateGroup {
                    states: [
                        State { 
                            name: "InList"
                            when: MyPhone.state == "rotated"
                            ParentChange { target: MyContent; parent: List }
                            SetProperties { target: MyContent; x: 133; y: 0; rotation: 90}
                        },
                        State { 
                            name: "InGrid"
                            when: MyPhone.state != "rotated"
                            ParentChange { target: MyContent; parent: Grid }
                            SetProperties { target: MyContent; x: 0; y: 0; }
                        }
                    ]
                    transitions: Transition {
                        fromState: "*"; toState: "*"; 
                        SequentialAnimation { 
                            ParentChangeAction{} 
                            NumericAnimation { properties: "x,y,rotation" } 
                        } 
                    }
                }

            }
        }

        Rect {
            width: 800
            height: 30
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
                y: 40
                width: 800
                height: 400
                column: 2
                model: Model.parts.grid
            }
        }

        Rect {
            width: 800
            height: 30
            anchors.bottom: parent.bottom
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
