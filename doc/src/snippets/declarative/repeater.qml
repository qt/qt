import Qt 4.6

//! [0]
Rectangle { 
    width: 220; height: 20; color: "white"
    Component { id: Dot
        Rectangle { width: 20; height: 20; radius: 10; color: "green" }
    }
    Row {
        Rectangle { width: 10; height: 20; color: "red" }
        Repeater { delegate: Dot; model: 10 }
        Rectangle { width: 10; height: 20; color: "blue" }
    }
}
//! [0]
