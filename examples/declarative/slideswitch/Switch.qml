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
        drag.target: Knob; drag.axis: "x"; drag.xmin: 1; drag.xmax: 78
    }
    states: [
        State {
            name: "On"
            SetProperty { target: Knob; property: "x"; value: 78 }
            SetProperty { target: Switch; property: "on"; value: true }
        },
        State {
            name: "Off"
            SetProperty { target: Knob; property: "x"; value: 1 }
            SetProperty { target: Switch; property: "on"; value: false }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "x"; easing: "easeInOutQuad"; duration: 200 }
        }
    ]
}
