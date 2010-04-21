//![0]
import Qt 4.7

Item {
    id: toggleswitch
    width: background.width; height: background.height

//![1]
    property bool on: false
//![1]

//![2]
    function toggle() {
        if (toggleswitch.state == "on")
            toggleswitch.state = "off";
        else toggleswitch.state = "on";
    }
//![2]

//![3]
    function dorelease() {
        if (knob.x == 1) {
            if (toggleswitch.state == "off") return;
        }
        if (knob.x == 78) {
            if (toggleswitch.state == "on") return;
        }
        toggle();
    }
//![3]

//![4]
    Image {
        id: background
        source: "background.svg"
        MouseArea { anchors.fill: parent; onClicked: toggle() }
    }
//![4]

//![5]
    Image {
        id: knob
        x: 1; y: 2
        source: "knob.svg"

        MouseArea {
            anchors.fill: parent
            drag.target: knob; drag.axis: "XAxis"; drag.minimumX: 1; drag.maximumX: 78
            onClicked: toggle()
            onReleased: dorelease()
        }
    }
//![5]

//![6]
    states: [
        State {
            name: "on"
            PropertyChanges { target: knob; x: 78 }
            PropertyChanges { target: toggleswitch; on: true }
        },
        State {
            name: "off"
            PropertyChanges { target: knob; x: 1 }
            PropertyChanges { target: toggleswitch; on: false }
        }
    ]
//![6]

//![7]
    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: "InOutQuad"; duration: 200 }
    }
//![7]
}
//![0]
