import Qt 4.6

Rectangle {
    id: Wrapper
    width: 240
    height: 320
    Rectangle {
        id: MyRect
        color: "red"
        width: 50; height: 50
        x: 100; y: 100
        MouseRegion {
            anchors.fill: parent
            onClicked: if (Wrapper.state == "state1") Wrapper.state = ""; else Wrapper.state = "state1";
        }
    }
    states: State {
        name: "state1"
        PropertyChanges { target: MyRect; x: 200; color: "blue" }
    }
    transitions: Transition {
        PropertyAnimation { properties: "x,color"; duration: 1000 } //x is real, color is color
    }
}
