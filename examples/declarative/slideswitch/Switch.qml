import Qt 4.6

Item {
    id: mySwitch
    width: groove.width; height: groove.height

    property var on

    Script {

        function toggle() {
            if(mySwitch.state == "On")
                mySwitch.state = "Off";
            else
                mySwitch.state = "On";
        }
        function dorelease() {
            if(knob.x == 1) {
                if(mySwitch.state == "Off")
                    return;
            }

            if(knob.x == 78) {
                if(mySwitch.state == "On")
                    return;
            }

            toggle();
        }

    }
    Image { id: groove; source: "background.svg" }
    MouseRegion { anchors.fill: groove; onClicked: { toggle() } }
    Image { id: knob; source: "knob.svg"; x: 1; y: 2 }
    MouseRegion {
        anchors.fill: knob
        onClicked: { toggle() }
        onReleased: { dorelease() }
        drag.target: knob; drag.axis: "XAxis"; drag.minimumX: 1; drag.maximumX: 78
    }
    states: [
        State {
            name: "On"
            PropertyChanges { target: knob; x: 78 }
            PropertyChanges { target: mySwitch; on: true }
        },
        State {
            name: "Off"
            PropertyChanges { target: knob; x: 1 }
            PropertyChanges { target: mySwitch; on: false }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "x"; easing: "easeInOutQuad"; duration: 200 }
        }
    ]
}
