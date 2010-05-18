import Qt 4.7
import "content"

Rectangle {
    width: 640; height: 240
    color: "#646464"

    Row {
        anchors.centerIn: parent
        Clock { city: "New York"; shift: -4 }
        Clock { city: "Mumbai"; shift: 5.5 }
        Clock { city: "Tokyo"; shift: 9 }
    }
}
