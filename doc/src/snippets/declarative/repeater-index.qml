import Qt 4.7

Rectangle {
    width: 50; height: childrenRect.height; color: "white"

//! [0]
    Column {
        Repeater {
            model: 10 
            Text { text: "I'm item " + index }
        }
    }
//! [0]
}

