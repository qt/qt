import Qt 4.7

Rectangle {
    id: page
    width: 420; height: 420

    Column {
        id: layout1
        y: 0
        move: Transition {
            NumberAnimation { properties: "y"; easing.type: "OutBounce" }
        }
        add: Transition {
            NumberAnimation { properties: "y"; easing.type: "OutQuad" }
        }

        Rectangle { color: "red"; width: 100; height: 50; border.color: "black"; radius: 15 }

        Rectangle {
            id: blueV1
            width: 100; height: 50
            color: "lightsteelblue"
            border.color: "black"
            radius: 15 
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "green"; width: 100; height: 50; border.color: "black"; radius: 15 }

        Rectangle {
            id: blueV2
            width: 100; height: 50
            color: "lightsteelblue"
            border.color: "black"
            radius: 15 
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "orange"; width: 100; height: 50; border.color: "black"; radius: 15 }
    }

    Row {
        id: layout2
        y: 300 
        move: Transition {
            NumberAnimation { properties: "x"; easing.type: "OutBounce" }
        }
        add: Transition {
            NumberAnimation { properties: "x"; easing.type: "OutQuad" }
        }

        Rectangle { color: "red"; width: 50; height: 100; border.color: "black"; radius: 15 }

        Rectangle {
            id: blueH1
            width: 50; height: 100
            color: "lightsteelblue"
            border.color: "black"
            radius: 15
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "green"; width: 50; height: 100; border.color: "black"; radius: 15 }

        Rectangle {
            id: blueH2
            width: 50; height: 100
            color: "lightsteelblue"
            border.color: "black"
            radius: 15 
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "orange"; width: 50; height: 100; border.color: "black"; radius: 15 }
    }

    Button {
        x: 135; y: 90
        text: "Remove"
        icon: "del.png"

        onClicked: {
            blueH2.opacity = 0
            blueH1.opacity = 0
            blueV1.opacity = 0
            blueV2.opacity = 0
            blueG1.opacity = 0
            blueG2.opacity = 0
            blueG3.opacity = 0
            blueF1.opacity = 0
            blueF2.opacity = 0
            blueF3.opacity = 0
        }
    }

    Button {
        x: 145; y: 140
        text: "Add"
        icon: "add.png"

        onClicked: {
            blueH2.opacity = 1
            blueH1.opacity = 1
            blueV1.opacity = 1
            blueV2.opacity = 1
            blueG1.opacity = 1
            blueG2.opacity = 1
            blueG3.opacity = 1
            blueF1.opacity = 1
            blueF2.opacity = 1
            blueF3.opacity = 1
        }
    }

    Grid {
        x: 260; y: 0
        columns: 3

        move: Transition {
            NumberAnimation { properties: "x,y"; easing.type: "OutBounce" }
        }

        add: Transition {
            NumberAnimation { properties: "x,y"; easing.type: "OutBounce" }
        }

        Rectangle { color: "red"; width: 50; height: 50; border.color: "black"; radius: 15 }

        Rectangle {
            id: blueG1
            width: 50; height: 50
            color: "lightsteelblue"
            border.color: "black"
            radius: 15 
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "green"; width: 50; height: 50; border.color: "black"; radius: 15 }

        Rectangle {
            id: blueG2
            width: 50; height: 50
            color: "lightsteelblue"
            border.color: "black"
            radius: 15 
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "orange"; width: 50; height: 50; border.color: "black"; radius: 15 }

        Rectangle {
            id: blueG3
            width: 50; height: 50
            color: "lightsteelblue"
            border.color: "black"
            radius: 15 
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "red"; width: 50; height: 50; border.color: "black"; radius: 15 }
        Rectangle { color: "green"; width: 50; height: 50; border.color: "black"; radius: 15 }
        Rectangle { color: "orange"; width: 50; height: 50; border.color: "black"; radius: 15 }
    }

    Flow {
        id: layout4
        x: 260; y: 250; width: 150

        move: Transition {
            NumberAnimation { properties: "x,y"; easing.type: "OutBounce" }
        }

        add: Transition {
            NumberAnimation { properties: "x,y"; easing.type: "OutBounce" }
        }

        Rectangle { color: "red"; width: 50; height: 50; border.color: "black"; radius: 15 }

        Rectangle {
            id: blueF1
            width: 60; height: 50
            color: "lightsteelblue"
            border.color: "black"
            radius: 15 
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "green"; width: 30; height: 50; border.color: "black"; radius: 15 }

        Rectangle {
            id: blueF2
            width: 60; height: 50
            color: "lightsteelblue"
            border.color: "black"
            radius: 15 
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "orange"; width: 50; height: 50; border.color: "black"; radius: 15 }

        Rectangle { 
            id: blueF3
            width: 40; height: 50
            color: "lightsteelblue"
            border.color: "black"
            radius: 15 
            Behavior on opacity { NumberAnimation {} }
        }

        Rectangle { color: "red"; width: 80; height: 50; border.color: "black"; radius: 15 }
    }

}
