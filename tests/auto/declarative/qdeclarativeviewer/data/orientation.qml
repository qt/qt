import Qt 4.7
Rectangle {
    color: "black"
    width: (runtime.orientation == Orientation.RightUp || runtime.orientation == Orientation.LeftUp) ? 300 : 200
    height: (runtime.orientation == Orientation.RightUp || runtime.orientation == Orientation.LeftUp) ? 200 : 300
    Text {
        text: {
            if (runtime.orientation == Orientation.TopUp)
                return "TopUp"
            if (runtime.orientation == Orientation.TopDown)
                return "TopDown"
            if (runtime.orientation == Orientation.LeftUp)
                return "LeftUp"
            if (runtime.orientation == Orientation.RightUp)
                return "RightUp"
        }
        color: "white"
    }
} 
