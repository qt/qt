import Qt 4.7

Rectangle {
    width: 220; height: 20; color: "white"

//! [0]
    Row {
        Rectangle { width: 10; height: 20; color: "red" }
        Repeater {
            model: 10 
            Rectangle { width: 20; height: 20; radius: 10; color: "green" }
        }
        Rectangle { width: 10; height: 20; color: "blue" }
    }
//! [0]
}
