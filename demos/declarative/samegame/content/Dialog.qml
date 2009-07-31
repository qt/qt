import Qt 4.6

Rect {
    id: page
    color: "white"; pen.width: 1; width: 200; height: 60;
    property string text: "Hello World!"
    opacity: 0
    opacity: Behavior { 
        SequentialAnimation {
            NumberAnimation {property: "opacity"; from: 0; duration: 1500 }
            NumberAnimation {property: "opacity"; to: 0;  duration: 1500 }
        }
    }
    Text { anchors.centerIn: parent; text: parent.text }
}
