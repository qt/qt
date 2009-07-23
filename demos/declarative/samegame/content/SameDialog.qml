import Qt 4.6

Rect {
    property string text: "Hello World!"
    property int show: 0
    id: page
    opacity: 0
    opacity: Behavior { 
        SequentialAnimation {
            NumberAnimation {property: "opacity"; duration: 1500 }
            NumberAnimation {property: "opacity"; to: 0;  duration: 1500 }
        }
    }
    color: "white"
    pen.width: 1
    width: 200
    height: 60
    Text { anchors.centeredIn: parent; text: parent.text }
}
