import Qt 4.6
Rectangle {
    width: 800;
    height: 480;
    color: "black";

    Rectangle {
        id: gr
        color: "green"
        width: 100; height: 100
    }

    MouseArea {
        id: mouser
        anchors.fill: parent
    }

    Rectangle {
        id: np
        x: 300
        width: 300; height: 300
        color: "yellow"
        clip: true
        Rectangle {
            color: "red"
            x: 100; y: 100; height: 100; width: 100
        }

    }

    Rectangle {
        id: vp
        x: 200; y: 200
        width: 100; height: 100
        color: "blue"
        rotation: 45
        scale: 2
    }

    states: State {
        name: "state1"
        when: mouser.pressed
        ParentChange {
            target: gr
            parent: np
            x: 100; y: 100; width: 200;
        }
    }

    transitions: Transition {
        reversible: true
        to: "state1"
        ParentAnimation {
            target: gr; via: vp;
            NumberAnimation { properties: "x,y,rotation,scale,width" }
        }
    }
}
