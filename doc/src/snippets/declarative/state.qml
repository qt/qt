//![0]
import Qt 4.7

Rectangle {
    id: myRect
    width: 100; height: 100
    color: "black"

    states: [
        State {
            name: "clicked"
            PropertyChanges {
                target: myRect
                color: "red"
            }
        }
    ]

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (myRect.state == "")     // i.e. the default state
                myRect.state = "clicked";
            else
                myRect.state = "";
        }
    }
}
//![0]
