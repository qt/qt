import Qt 4.6

Item {
    id: Switch
    width: Groove.width; height: Groove.height

    property var on

    Script {

        function toggle() {
            if(Switch.state == "On")
                Switch.state = "Off";
            else
                Switch.state = "On";
        }
        function dorelease() {
            if(Knob.x == 1) {
                if(Switch.state == "Off") 
                    return;
            }

            if(Knob.x == 78) {
                if(Switch.state == "On") 
                    return;
            }

            toggle();
        }
    
    }
    Image { id: Groove; source: "background.svg" }
    MouseRegion { anchors.fill: Groove; onClicked: { toggle() } }
    Image { id: Knob; source: "knob.svg"; x: 1; y: 2 }
    MouseRegion {
        anchors.fill: Knob
        onClicked: { toggle() }
        onReleased: { dorelease() }
        drag.target: Knob; drag.axis: "XAxis"; drag.minimumX: 1; drag.maximumX: 78
    }
    states: [
        State {
            name: "On"
            PropertyChanges { target: Knob; x: 78 }
            PropertyChanges { target: Switch; on: true }
        },
        State {
            name: "Off"
            PropertyChanges { target: Knob; x: 1 }
            PropertyChanges { target: Switch; on: false }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "x"; easing: "easeInOutQuad"; duration: 200 }
        }
    ]
}
