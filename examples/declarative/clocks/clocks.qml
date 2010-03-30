import Qt 4.6
import "content"

Rectangle {
    width: childrenRect.width
    height: childrenRect.height
    color: "#646464"

    Grid {
        columns: 3
        Clock { city: "New York"; shift: -4 }
        Clock { city: "Mumbai"; shift: 5.5 }
        Clock { city: "Tokyo"; shift: 9 }
    }
}
