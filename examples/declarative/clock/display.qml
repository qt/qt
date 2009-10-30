import Qt 4.6

Rectangle {
    width: childrenRect.width
    height: childrenRect.height
    color: "#646464"

    Grid {
        columns: 3
        Clock { city: "New York"; shift: -4 }
        Clock { city: "London" }
        Clock { city: "Brisbane"; shift: 10 }
    }
}
