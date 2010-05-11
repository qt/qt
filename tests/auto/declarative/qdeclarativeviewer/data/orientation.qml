import Qt 4.7
Rectangle {
    color: "black"
    width: (runtime.orientation == Orientation.Landscape) ? 300 : 200
    height: (runtime.orientation == Orientation.Landscape) ? 200 : 300
    Text {
        text: runtime.orientation == Orientation.Landscape ? "Landscape" : "Portrait"
        color: "white"
    }
} 