//![0]
import Qt 4.7

Item {
    id: window
    width: 200; height: 200

    Rectangle { id: myRect; width: 100; height: 100; color: "red" }

    states: State {
        name: "reanchored"

        AnchorChanges {
            target: myRect
            anchors.top: window.top
            anchors.bottom: window.bottom
        }
        PropertyChanges {
            target: myRect
            anchors.topMargin: 3
            anchors.bottomMargin: 3
        }
    }

    MouseArea { anchors.fill: parent; onClicked: window.state = "reanchored" }
}
//![0]

